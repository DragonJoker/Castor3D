#include "Castor3D/Render/Opaque/Lighting/LightsPipeline.hpp"

#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
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

	LightsPipeline::LightsPipeline( Scene const & scene
		, Camera const & camera
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightPipelineConfig const & config
		, LightPassResult const & lpResult
		, ShadowMapResult const & smResult
		, LightRenderPassArray const & renderPasses
		, LightRenderPassArray const & stencilRenderPasses
		, crg::ImageViewIdArray const & targetColourResult )
		: m_context{ context }
		, m_smResult{ smResult }
		, m_device{ device }
		, m_renderPasses{ renderPasses }
		, m_scene{ scene }
		, m_targetCamera{ camera }
		, m_config{ config }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, "Vtx" + m_config.getName( *m_scene.getEngine() )
			, LightPass::getVertexShaderSource( m_config.lightType ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, "Pxl" + m_config.getName( *m_scene.getEngine() )
			, LightPass::getPixelShaderSource( m_config.lightingModelId
				, m_scene.getBackgroundModelId()
				, m_scene
				, m_config.sceneFlags
				, m_config.lightType
				, m_config.shadowType
				, m_config.shadows
				, makeExtent2D( getExtent( targetColourResult.front() ) ) ) }
		, m_stages{ makeShaderState( m_device, m_vertexShader )
			, makeShaderState( m_device, m_pixelShader ) }
		, m_descriptorLayout{ doCreateDescriptorLayout() }
		, m_descriptorPool{ m_descriptorLayout->createPool( MaxLightsCount ) }
		, m_lightPipeline{ pass
			, context
			, graph
			, m_config
			, m_renderPasses
			, m_stages
			, *m_descriptorLayout
			, makeExtent2D( getExtent( targetColourResult.front() ) ) }
		, m_stencilRenderPasses{ stencilRenderPasses }
		, m_stencilPipeline{ m_config.lightType == LightType::eDirectional
			? nullptr
			: std::make_unique< StencilPipeline >( pass
				, context
				, graph
				, m_device
				, m_config
				, stencilRenderPasses
				, *m_descriptorLayout
				, makeExtent2D( getExtent( targetColourResult.front() ) ) ) }
		, m_vertexBuffer{ m_config.lightType == LightType::eSpot
			? GpuBufferOffsetT< float >{}
			: doCreateVertexBuffer( nullptr ) }
		, m_viewport{ *device.renderSystem.getEngine() }
		, m_targetColourResult{ targetColourResult }
	{
		m_viewport.setOrtho( -1, 1, -1, 1, -1, 1 );
	}

	void LightsPipeline::clear()
	{
		m_viewport.resize( makeSize( getExtent( m_targetColourResult.front() ) ) );
		m_viewport.update();
		m_enabledLights.clear();
	}

	void LightsPipeline::updateCamera( Camera const & camera )
	{
		m_camera = &camera;
	}

	void LightsPipeline::addLight( Light const & light )
	{
		auto camera = m_camera ? m_camera : &m_targetCamera;
		auto & entry = doCreateLightEntry( light );

		if ( m_config.lightType != LightType::eDirectional )
		{
			entry.cameraUbo.cpuUpdate( *camera
				, 0u
				, false );
			auto model = doComputeModelMatrix( light, *camera );
			auto & data = entry.modelMatrixUbo.getData();
			data.prvModel = data.curModel;
			data.curModel = model;
		}
		else
		{
			entry.cameraUbo.cpuUpdate( *camera
				, camera->getView()
				, m_viewport.getProjection()
				, 0u
				, true );
		}

		m_enabledLights.push_back( &entry );
	}

	void LightsPipeline::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex
		, uint32_t & pipelineIndex )
	{
		if ( !m_enabledLights.empty() )
		{
			auto baseDS = m_lightPipeline.getDescriptorSet();
			doRecordLightPass( m_renderPasses[getLightRenderPassIndex( pipelineIndex != 0u, m_config.lightType )]
				, m_stencilRenderPasses[pipelineIndex]
				, baseDS
				, 0u
				, context
				, commandBuffer
				, passIndex
				, pipelineIndex );
			pipelineIndex = 1u;

			if ( m_enabledLights.size() > 1u )
			{
				auto & renderPass = m_renderPasses[getLightRenderPassIndex( true, m_config.lightType )];
				auto & stencilRenderPass = m_stencilRenderPasses[pipelineIndex];

				for ( size_t i = 1u; i < m_enabledLights.size(); ++i )
				{
					doRecordLightPass( renderPass
						, stencilRenderPass
						, baseDS
						, i
						, context
						, commandBuffer
						, passIndex
						, pipelineIndex );
				}
			}
		}
	}

	ashes::DescriptorSetLayoutPtr LightsPipeline::doCreateDescriptorLayout()
	{
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eLights )
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
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eSmLinearCmp )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eSmVariance )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eRandomStorage )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( LightPassLgtIdx::eCount );
		PipelineFlags flags{ PassComponentCombine{}, m_config.lightingModelId, {} };
		m_scene.getBackground()->addBindings( flags, setLayoutBindings, index );

		return m_device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
	}

	LightDescriptors & LightsPipeline::doCreateLightEntry( Light const & light )
	{
		auto ires = m_lightDescriptors.emplace( light.getBufferIndex(), nullptr );

		if ( ires.second )
		{
			ires.first->second = std::make_unique< LightDescriptors >( light, m_device );
			auto & result = *ires.first->second;
			auto & scene = *light.getScene();
			ashes::WriteDescriptorSetArray writes;
			writes.emplace_back( scene.getLightCache().getBinding( uint32_t( LightPassLgtIdx::eLights ) ) );
			writes.emplace_back( result.cameraUbo.getDescriptorWrite( uint32_t( LightPassLgtIdx::eMatrix ) ) );

			if ( m_config.lightType != LightType::eDirectional )
			{
				result.modelMatrixUbo = m_device.uboPool->getBuffer< ModelBufferConfiguration >( 0u );
				writes.emplace_back( result.modelMatrixUbo.getDescriptorWrite( uint32_t( LightPassLgtIdx::eModelMatrix ) ) );
			}

			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmLinear )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { m_device.renderSystem.getEngine()->getDefaultSampler()->getSampler()
					, m_smResult[SmTexture::eLinearDepth].wholeView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmLinearCmp )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { *m_smResult[SmTexture::eLinearDepth].sampler
					, m_smResult[SmTexture::eLinearDepth].wholeView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmVariance )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { m_device.renderSystem.getEngine()->getDefaultSampler()->getSampler()
					, m_smResult[SmTexture::eVariance].wholeView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
			auto & randomStorage = m_device.renderSystem.getRandomStorage().getBuffer();
			writes.emplace_back( uint32_t( LightPassLgtIdx::eRandomStorage )
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, ashes::VkDescriptorBufferInfoArray{ { randomStorage
					, 0u
					, randomStorage.getSize() } } );

			auto index = uint32_t( LightPassLgtIdx::eCount );
			PipelineFlags flags{ PassComponentCombine{}, m_config.lightingModelId, {} };
			m_scene.getBackground()->addDescriptors( flags, writes, m_targetColourResult, index );

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
			std::memcpy( buffer.data(), data.data()->constPtr(), size_t( result.getSize() ) );
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
		std::memcpy( buffer.data(), data.data()->constPtr(), size_t( vertexBuffer.getSize() ) );
		vertexBuffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
	}

	void LightsPipeline::doRecordLightPassBase( LightRenderPass const & renderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex
		, uint32_t pipelineIndex )
	{
		m_context.vkCmdBeginDebugBlock( commandBuffer
			, { "Light" + std::to_string( lightIndex ) + "/Lighting"
				, m_context.getNextRainbowColour() } );
		auto & entry = *m_enabledLights[lightIndex];
		VkBuffer vertexBuffer{ entry.vertexBuffer.getBuffer() };
		VkDeviceSize offset{ entry.vertexBuffer.getOffset() };
		VkDescriptorSet specDS = *entry.descriptorSet;
		auto beginRenderPass = makeVkStruct< VkRenderPassBeginInfo >( *renderPass.renderPass
			, *renderPass.framebuffers[passIndex].fbo
			, VkRect2D{ {}, renderPass.framebuffers[passIndex].fbo->getDimensions() }
			, uint32_t( renderPass.clearValues.size() )
			, renderPass.clearValues.data() );
		m_context.vkCmdBeginRenderPass( commandBuffer
			, &beginRenderPass
			, VK_SUBPASS_CONTENTS_INLINE );
		auto pipeline = m_lightPipeline.getPipeline( pipelineIndex );
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
		auto bufferIndex = entry.light.getBufferIndex();
		m_context.vkCmdPushConstants( commandBuffer
			, m_lightPipeline.getPipelineLayout()
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, 0u
			, 4u
			, &bufferIndex );
		m_context.vkCmdDraw( commandBuffer
			, m_count
			, 1u
			, 0u
			, 0u );
		m_context.vkCmdEndRenderPass( commandBuffer );
		m_context.vkCmdEndDebugBlock( commandBuffer );

		for ( auto & attach : renderPass.framebuffers[passIndex].attaches )
		{
			if ( attach.view.size() == 1u )
			{
				auto view = attach.view.front();
				context.setLayoutState( view
					, { attach.output
					, crg::getAccessMask( attach.output )
					, crg::getStageMask( attach.output ) } );
			}
		}
	}

	void LightsPipeline::doRecordStencilPrePass( LightRenderPass const & renderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex )
	{
		m_context.vkCmdBeginDebugBlock( commandBuffer
			, { "Light" + std::to_string( lightIndex ) + "/Stencil"
				, m_context.getNextRainbowColour() } );
		auto & entry = *m_enabledLights[lightIndex];
		VkBuffer vertexBuffer{ entry.vertexBuffer.getBuffer() };
		VkDeviceSize offset{ entry.vertexBuffer.getOffset() };
		VkDescriptorSet specDS = *entry.descriptorSet;
		auto beginRenderPass = makeVkStruct< VkRenderPassBeginInfo >( *renderPass.renderPass
			, *renderPass.framebuffers[passIndex].fbo
			, VkRect2D{ {}, renderPass.framebuffers[passIndex].fbo->getDimensions() }
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

	void LightsPipeline::doRecordLightPass( LightRenderPass const & renderPass
		, LightRenderPass const & stencilRenderPass
		, VkDescriptorSet baseDS
		, size_t lightIndex
		, crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t passIndex
		, uint32_t pipelineIndex )
	{
		m_context.vkCmdBeginDebugBlock( commandBuffer
			, { "Light" + std::to_string( lightIndex ) + m_config.getName( *m_scene.getEngine() )
				, m_context.getNextRainbowColour() } );

		if ( m_config.lightType != LightType::eDirectional )
		{
			CU_Require( m_stencilPipeline );
			doRecordStencilPrePass( stencilRenderPass
				, baseDS
				, lightIndex
				, context
				, commandBuffer
				, passIndex );
		}

		doRecordLightPassBase( renderPass
			, baseDS
			, lightIndex
			, context
			, commandBuffer
			, passIndex
			, pipelineIndex );
		m_context.vkCmdEndDebugBlock( commandBuffer );
	}

	//*********************************************************************************************
}
