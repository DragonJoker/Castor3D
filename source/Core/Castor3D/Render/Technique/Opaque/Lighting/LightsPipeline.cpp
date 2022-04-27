#include "Castor3D/Render/Technique/Opaque/Lighting/LightsPipeline.hpp"

#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"

namespace castor3d
{
	namespace drlgtpass
	{
		static float calcLightScale( const castor3d::SpotLight & light
			, float max )
		{
			return getMaxDistance( light
				, light.getAttenuation()
				, max );
		}

		static float calcLightScale( const castor3d::PointLight & light
			, float max )
		{
			return getMaxDistance( light
				, light.getAttenuation()
				, max );
		}
	}

	//*********************************************************************************************

	LightsPipeline::LightsPipeline( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightPipelineConfig const & config
		, LightPassResult const & lpResult
		, ShadowMapResult const & smResult
		, std::vector< LightRenderPass > const & renderPasses
		, std::vector< LightRenderPass > const & stencilRenderPasses )
		: m_context{ context }
		, m_smResult{ smResult }
		, m_device{ device }
		, m_renderPasses{ renderPasses }
		, m_config{ config }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, castor::string::snakeToCamelCase( getName( m_config.lightType ) )
			, LightPass::getVertexShaderSource( m_config.lightType ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, castor::string::snakeToCamelCase( getName( m_config.lightType ) )
			, LightPass::getPixelShaderSource( m_config.passType
				, m_device.renderSystem
				, m_config.sceneFlags
				, m_config.lightType
				, m_config.shadowType
				, m_config.shadows ) }
		, m_stages{ makeShaderState( m_device, m_vertexShader )
			, makeShaderState( m_device, m_pixelShader ) }
		, m_descriptorLayout{ doCreateDescriptorLayout() }
		, m_descriptorPool{ m_descriptorLayout->createPool( MaxLightsCount ) }
		, m_lightPipeline{ pass, context, graph, m_config, m_renderPasses, m_stages, *m_descriptorLayout }
		, m_stencilRenderPasses{ stencilRenderPasses }
		, m_stencilPipeline{ m_config.lightType == LightType::eDirectional
			? nullptr
			: std::make_unique< StencilPipeline >( pass, context, graph, m_device, m_config, stencilRenderPasses, *m_descriptorLayout ) }
		, m_vertexBuffer{ m_config.lightType == LightType::eSpot
			? GpuBufferOffsetT< float >{}
			: doCreateVertexBuffer( nullptr ) }
		, m_viewport{ *device.renderSystem.getEngine() }
	{
		m_viewport.setOrtho( -1, 1, -1, 1, -1, 1 );
	}

	void LightsPipeline::clear()
	{
		m_viewport.resize( makeSize( m_renderPasses[0].framebuffer->getDimensions() ) );
		m_viewport.update();
		m_enabledLights.clear();
	}

	void LightsPipeline::addLight( Camera const & camera
		, Light const & light )
	{
		auto & entry = doCreateLightEntry( light );

		if ( m_config.lightType != LightType::eDirectional )
		{
			entry.matrixUbo.cpuUpdate( camera.getView()
				, camera.getProjection( false ) );
			auto model = doComputeModelMatrix( light, camera );
			auto & data = entry.modelMatrixUbo.getData();
			data.prvModel = data.curModel;
			data.curModel = model;
		}
		else
		{
			entry.matrixUbo.cpuUpdate( camera.getView()
				, m_viewport.getProjection() );
		}

		m_enabledLights.push_back( &entry );
	}

	void LightsPipeline::removeLight( Camera const & camera
		, Light const & light )
	{
		auto it = std::find_if( m_enabledLights.begin()
			, m_enabledLights.end()
			, [&light]( LightDescriptors const * lookup )
			{
				return &lookup->light == &light;
			} );

		if ( it != m_enabledLights.end() )
		{
			m_enabledLights.erase( it );
		}
	}

	void LightsPipeline::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t & index )
	{
		if ( !m_enabledLights.empty() )
		{
			auto baseDS = m_lightPipeline.getDescriptorSet();
			doRecordLightPass( m_renderPasses[getLightRenderPassIndex( index != 0u, m_config.lightType )]
				, m_stencilRenderPasses[index]
				, baseDS
				, 0u
				, context
				, commandBuffer
				, index );
			index = 1u;

			if ( m_enabledLights.size() > 1u )
			{
				auto & renderPass = m_renderPasses[getLightRenderPassIndex( true, m_config.lightType )];
				auto & stencilRenderPass = m_stencilRenderPasses[index];

				for ( size_t i = 1u; i < m_enabledLights.size(); ++i )
				{
					doRecordLightPass( renderPass
						, stencilRenderPass
						, baseDS
						, i
						, context
						, commandBuffer
						, index );
				}
			}
		}
	}

	ashes::DescriptorSetLayoutPtr LightsPipeline::doCreateDescriptorLayout()
	{
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eLight )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );

		if ( m_config.lightType != LightType::eDirectional )
		{
			setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eModelMatrix )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eSmLinear )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eSmVariance )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		return m_device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
	}

	LightDescriptors & LightsPipeline::doCreateLightEntry( Light const & light )
	{
		auto ires = m_lightDescriptors.emplace( light.getBufferIndex(), nullptr );

		if ( ires.second )
		{
			ires.first->second = std::make_unique< LightDescriptors >( light, m_device );
			auto & result = *ires.first->second;
			auto lightSize = shader::getMaxLightComponentsCount() * sizeof( castor::Point4f );
			auto & scene = *light.getScene();
			ashes::WriteDescriptorSetArray writes;
			writes.emplace_back( scene.getLightCache().getBinding( uint32_t( LightPassLgtIdx::eLight )
				, light.getBufferIndex() * lightSize
				, lightSize ) );
			writes.emplace_back( result.matrixUbo.getDescriptorWrite( uint32_t( LightPassLgtIdx::eMatrix ) ) );

			if ( m_config.lightType != LightType::eDirectional )
			{
				result.modelMatrixUbo = m_device.uboPool->getBuffer< ModelBufferConfiguration >( 0u );
				writes.emplace_back( result.modelMatrixUbo.getDescriptorWrite( uint32_t( LightPassLgtIdx::eModelMatrix ) ) );
			}

			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmLinear )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { m_device.renderSystem.getEngine()->getDefaultSampler().lock()->getSampler()
					, m_smResult[SmTexture::eLinearDepth].wholeView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmVariance )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { m_device.renderSystem.getEngine()->getDefaultSampler().lock()->getSampler()
					, m_smResult[SmTexture::eVariance].wholeView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );

			result.descriptorSet = m_descriptorPool->createDescriptorSet( 1u );
			result.descriptorSet->setBindings( writes );
			result.descriptorSet->update();

			if ( m_config.lightType == LightType::eSpot )
			{
				result.angle = uint32_t( std::ceil( light.getSpotLight()->getOuterCutOff().degrees() ) );
				result.vertexBuffer = doCreateVertexBuffer( &light );
			}
			else
			{
				result.vertexBuffer = m_vertexBuffer;
			}
		}
		else if ( m_config.lightType == LightType::eSpot )
		{
			auto & result = *ires.first->second;
			auto angle = uint32_t( std::ceil( light.getSpotLight()->getOuterCutOff().degrees() ) );

			if ( angle != result.angle )
			{
				result.angle = angle;
				doUpdateVertexBuffer( result.vertexBuffer
					, result.angle );
			}
		}

		return *ires.first->second;
	}

	castor::Matrix4x4f LightsPipeline::doComputeModelMatrix( castor3d::Light const & light
		, Camera const & camera )const
	{
		auto lightPos = light.getParent()->getDerivedPosition();
		auto camPos = camera.getParent()->getDerivedPosition();
		auto farZ = camera.getFar();
		castor::Matrix4x4f model{ 1.0f };

		if ( m_config.lightType == LightType::ePoint )
		{
			auto scale = drlgtpass::calcLightScale( *light.getPointLight()
				, float( farZ - castor::point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
			model = castor::matrix::setTransform( model
				, lightPos
				, castor::Point3f{ scale, scale, scale }
				, castor::Quaternion::identity() );
		}
		else
		{
			auto scale = drlgtpass::calcLightScale( *light.getSpotLight()
				, float( farZ - castor::point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
			model = castor::matrix::setTransform( model
				, lightPos
				, castor::Point3f{ scale, scale, scale }
				, light.getParent()->getDerivedOrientation() );
		}

		return model;
	}

	GpuBufferOffsetT< float > LightsPipeline::doCreateVertexBuffer( Light const * light )
	{
		GpuBufferOffsetT< float > result;

		if ( m_config.lightType == LightType::eDirectional )
		{
			float data[] =
			{
				-1.0f, -1.0f,
				-1.0f, +1.0f,
				+1.0f, -1.0f,
				+1.0f, -1.0f,
				-1.0f, +1.0f,
				+1.0f, +1.0f,
			};
			m_count = 6u;
			result = m_device.bufferPool->getBuffer< float >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				, 12u
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			auto buffer = result.getData();
			std::memcpy( buffer.data(), data, sizeof( data ) );
		}
		else
		{
			auto & data = m_config.lightType == LightType::ePoint
				? PointLight::generateVertices()
				: SpotLight::generateVertices( uint32_t( std::ceil( light->getSpotLight()->getOuterCutOff().degrees() ) ) );
			m_count = uint32_t( data.size() );
			result = m_device.bufferPool->getBuffer< float >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				, uint32_t( data.size() * 3u )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			auto buffer = result.getData();
			std::memcpy( buffer.data(), data.data()->constPtr(), result.getSize() );
		}

		result.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		return result;
	}

	void LightsPipeline::doUpdateVertexBuffer( GpuBufferOffsetT< float > & vertexBuffer
		, uint32_t angle )
	{
		auto & data = SpotLight::generateVertices( angle );
		auto buffer = vertexBuffer.getData();
		std::memcpy( buffer.data(), data.data()->constPtr(), vertexBuffer.getSize() );
		vertexBuffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
	}

	void LightsPipeline::doRecordLightPassBase( LightRenderPass const & renderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex )
	{
		m_context.vkCmdBeginDebugBlock( commandBuffer
			, { "Light" + std::to_string( lightIndex ) + "/Lighting"
				, m_context.getNextRainbowColour() } );
		auto & entry = *m_enabledLights[lightIndex];
		VkBuffer vertexBuffer{ entry.vertexBuffer.getBuffer() };
		VkDeviceSize offset{ entry.vertexBuffer.getOffset() };
		VkDescriptorSet specDS = *entry.descriptorSet;
		auto beginRenderPass = makeVkStruct< VkRenderPassBeginInfo >( *renderPass.renderPass
			, *renderPass.framebuffer
			, VkRect2D{ {}, renderPass.framebuffer->getDimensions() }
			, uint32_t( renderPass.clearValues.size() )
			, renderPass.clearValues.data() );
		m_context.vkCmdBeginRenderPass( commandBuffer
			, &beginRenderPass
			, VK_SUBPASS_CONTENTS_INLINE );
		auto pipeline = m_lightPipeline.getPipeline( passIndex );
		m_context.vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, pipeline );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, m_lightPipeline.getPipelineLayout()
			, 0u
			, 1u
			, &baseDS
			, 0u
			, nullptr );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, m_lightPipeline.getPipelineLayout()
			, 1u
			, 1u
			, &specDS
			, 0u
			, nullptr );
		m_context.vkCmdBindVertexBuffers( commandBuffer
			, 0u
			, 1u
			, &vertexBuffer
			, &offset );
		m_context.vkCmdDraw( commandBuffer
			, m_count
			, 1u
			, 0u
			, 0u );
		m_context.vkCmdEndRenderPass( commandBuffer );
		m_context.vkCmdEndDebugBlock( commandBuffer );

		for ( auto & attach : renderPass.attaches )
		{
			context.setLayoutState( attach.view
				, { attach.output
				, crg::getAccessMask( attach.output )
				, crg::getStageMask( attach.output ) } );
		}
	}

	void LightsPipeline::doRecordDirectionalLightPass( LightRenderPass const & renderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex )
	{
		doRecordLightPassBase( renderPass
			, baseDS
			, lightIndex
			, context
			, commandBuffer
			, passIndex );
		m_context.vkCmdEndDebugBlock( commandBuffer );
	}

	void LightsPipeline::doRecordStencilPrePass( LightRenderPass const & renderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer )
	{
		m_context.vkCmdBeginDebugBlock( commandBuffer
			, { "Light" + std::to_string( lightIndex ) + "/Stencil"
				, m_context.getNextRainbowColour() } );
		auto & entry = *m_enabledLights[lightIndex];
		VkBuffer vertexBuffer{ entry.vertexBuffer.getBuffer() };
		VkDeviceSize offset{ entry.vertexBuffer.getOffset() };
		VkDescriptorSet specDS = *entry.descriptorSet;
		auto beginRenderPass = makeVkStruct< VkRenderPassBeginInfo >( *renderPass.renderPass
			, *renderPass.framebuffer
			, VkRect2D{ {}, renderPass.framebuffer->getDimensions() }
			, uint32_t( renderPass.clearValues.size() )
			, renderPass.clearValues.data() );
		m_context.vkCmdBeginRenderPass( commandBuffer
			, &beginRenderPass
			, VK_SUBPASS_CONTENTS_INLINE );
		auto pipeline = m_stencilPipeline->getPipeline();
		m_context.vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, pipeline );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, m_stencilPipeline->getPipelineLayout()
			, 0u
			, 1u
			, &baseDS
			, 0u
			, nullptr );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, m_stencilPipeline->getPipelineLayout()
			, 1u
			, 1u
			, &specDS
			, 0u
			, nullptr );
		m_context.vkCmdBindVertexBuffers( commandBuffer
			, 0u
			, 1u
			, &vertexBuffer
			, &offset );
		m_context.vkCmdDraw( commandBuffer
			, m_count
			, 1u
			, 0u
			, 0u );
		m_context.vkCmdEndRenderPass( commandBuffer );
		m_context.vkCmdEndDebugBlock( commandBuffer );
	}

	void LightsPipeline::doRecordMeshLightPass( LightRenderPass const & renderPass
		, LightRenderPass const & stencilRenderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex )
	{
		CU_Require( m_stencilPipeline );
		doRecordStencilPrePass( stencilRenderPass
			, baseDS
			, lightIndex
			, context
			, commandBuffer );
		doRecordLightPassBase( renderPass
			, baseDS
			, lightIndex
			, context
			, commandBuffer
			, passIndex );
	}

	void LightsPipeline::doRecordLightPass( LightRenderPass const & renderPass
		, LightRenderPass const & stencilRenderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex )
	{
		m_context.vkCmdBeginDebugBlock( commandBuffer
			, { castor::string::snakeToCamelCase( getName( m_config.lightType ) ) + "Light" + std::to_string( lightIndex )
				, m_context.getNextRainbowColour() } );

		if ( m_config.lightType == LightType::eDirectional )
		{
			doRecordDirectionalLightPass( renderPass
				, baseDS
				, lightIndex
				, context
				, commandBuffer
				, passIndex );
		}
		else
		{
			doRecordMeshLightPass( renderPass
				, stencilRenderPass
				, baseDS
				, lightIndex
				, context
				, commandBuffer
				, passIndex );
		}

		m_context.vkCmdEndDebugBlock( commandBuffer );
	}

	//*********************************************************************************************
}
