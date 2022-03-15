#include "Castor3D/Render/Technique/Opaque/LightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Math/TransformationMatrix.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, LightingPass )

namespace castor3d
{
	namespace
	{
		castor::Point2f doCalcSpotLightBCone( const castor3d::SpotLight & light
			, float max )
		{
			float length{ getMaxDistance( light
				, light.getAttenuation()
				, max ) };
			float width{ light.getCutOff().degrees() / ( 45.0f ) };
			return castor::Point2f{ length * width, length };
		}

		float doCalcPointLightBSphere( const castor3d::PointLight & light
			, float max )
		{
			return getMaxDistance( light
				, light.getAttenuation()
				, max );
		}
	}

	//*********************************************************************************************

	LightPipelineConfig::LightPipelineConfig( PassTypeID passType
		, SceneFlags const & sceneFlags
		, Light const & light )
		: passType{ passType }
		, sceneFlags{ sceneFlags }
		, lightType{ light.getLightType() }
		, shadowType{ light.getShadowType() }
		, shadows{ shadowType != ShadowType::eNone }
	{
	}

	size_t LightPipelineConfig::makeHash()const
	{
		size_t hash = std::hash< PassTypeID >{}( passType );
		hash = castor::hashCombine( hash, uint32_t( sceneFlags ) );
		hash = castor::hashCombine( hash, uint32_t( lightType ) );
		hash = castor::hashCombine( hash, uint32_t( shadowType ) );
		hash = castor::hashCombine( hash, shadows );
		return hash;
	}

	//*********************************************************************************************

	LightDescriptors::LightDescriptors( Light const & plight
		, RenderDevice const & device )
		: light{ plight }
		, matrixUbo{ device }
	{
	}

	//*********************************************************************************************

	LightPipeline::LightPipeline( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, LightPipelineConfig const & config
		, std::vector< LightRenderPass > const & renderPasses
		, ashes::PipelineShaderStageCreateInfoArray stages
		, VkDescriptorSetLayout descriptorSetLayout )
		: m_holder{ pass
			, context
			, graph
			, crg::pp::Config{ std::vector< crg::VkPipelineShaderStageCreateInfoArray >{ crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages )
					, crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) }
				, std::vector< VkDescriptorSetLayout >{ descriptorSetLayout } }
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, 2u }
		, m_config{ config }
		, m_renderPasses{ renderPasses }
	{
		m_holder.initialise();
		doCreatePipeline();
	}

	void LightPipeline::doCreatePipeline()
	{
		ashes::PipelineVertexInputStateCreateInfo vertexLayout = doCreateVertexLayout();
		crg::VkVertexInputAttributeDescriptionArray vertexAttribs;
		crg::VkVertexInputBindingDescriptionArray vertexBindings;
		ashes::PipelineDepthStencilStateCreateInfo depthStencil{ 0u, false, false };
		auto iaState = makeVkStruct< VkPipelineInputAssemblyStateCreateInfo >( 0u
			, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, VK_FALSE );
		auto msState = makeVkStruct< VkPipelineMultisampleStateCreateInfo >( 0u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_FALSE
			, 0.0f
			, nullptr
			, VK_FALSE
			, VK_FALSE );
		auto rsState = makeVkStruct< VkPipelineRasterizationStateCreateInfo >( 0u
			, VK_FALSE
			, VK_FALSE
			, VK_POLYGON_MODE_FILL
			, doGetCullMode()
			, VK_FRONT_FACE_COUNTER_CLOCKWISE
			, VK_FALSE
			, 0.0f
			, 0.0f
			, 0.0f
			, 0.0f );
		VkPipelineVertexInputStateCreateInfo const & vsState = vertexLayout;
		VkPipelineDepthStencilStateCreateInfo const & dsState = depthStencil;

		for ( uint32_t index = 0u; index < 2u; ++index )
		{
			auto viewportState = doCreateViewportState( *m_renderPasses[index].framebuffer );
			auto colourBlend = doCreateBlendState( index != 0u );
			auto & program = m_holder.getProgram( index );
			auto & pipeline = m_holder.getPipeline( index );
			VkPipelineColorBlendStateCreateInfo const & cbState = colourBlend;
			VkPipelineViewportStateCreateInfo const & vpState = viewportState;
			auto createInfo = makeVkStruct< VkGraphicsPipelineCreateInfo >( 0u
				, uint32_t( program.size() )
				, program.data()
				, &vsState
				, &iaState
				, nullptr
				, &vpState
				, &rsState
				, &msState
				, &dsState
				, &cbState
				, nullptr
				, m_holder.getPipelineLayout()
				, *m_renderPasses[index].renderPass
				, 0u
				, VK_NULL_HANDLE
				, 0 );
			auto res = m_holder.getContext().vkCreateGraphicsPipelines( m_holder.getContext().device
				, m_holder.getContext().cache
				, 1u
				, &createInfo
				, m_holder.getContext().allocator
				, &pipeline );
			crg::checkVkResult( res, m_holder.getPass().getGroupName() + " - Pipeline creation" );
			crgRegisterObject( m_holder.getContext(), m_holder.getPass().getGroupName(), pipeline );
		}
	}

	ashes::PipelineVertexInputStateCreateInfo LightPipeline::doCreateVertexLayout()
	{
		if ( m_config.lightType == LightType::eDirectional )
		{
			return { 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 0u
					, sizeof( castor::Point2f )
					, VK_VERTEX_INPUT_RATE_VERTEX } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
					, 0u
					, VK_FORMAT_R32G32_SFLOAT
					, 0u } } };
		}

		return { 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( castor::Point3f )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
				, 0u
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } } };
	}

	ashes::PipelineViewportStateCreateInfo LightPipeline::doCreateViewportState( ashes::FrameBuffer const & framebuffer )
	{
		auto dim = framebuffer.getDimensions();
		ashes::VkViewportArray viewports{ { 0.0f
			, 0.0f
			, float( dim.width )
			, float( dim.height )
			, 0.0f
			, 1.0f } };
		ashes::VkScissorArray scissors{ { {}, dim } };
		return { 0u
			, 1u
			, viewports
			, 1u
			, scissors };
	}

	ashes::PipelineColorBlendStateCreateInfo LightPipeline::doCreateBlendState( bool blend )
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachs;

		if ( blend )
		{
			attachs.push_back( { VK_TRUE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, defaultColorWriteMask } );
		}
		else
		{
			attachs.push_back( { VK_FALSE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ZERO
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ZERO
				, VK_BLEND_OP_ADD
				, defaultColorWriteMask } );
		}

		attachs.push_back( attachs.back() );

		return { 0u
			, VK_FALSE
			, VK_LOGIC_OP_COPY
			, std::move( attachs ) };
	}

	VkCullModeFlags LightPipeline::doGetCullMode()const
	{
		if ( m_config.lightType == LightType::eDirectional )
		{
			return VK_CULL_MODE_NONE;
		}

		return VK_CULL_MODE_BACK_BIT;
	}

	//*********************************************************************************************

	LightsPipeline::LightsPipeline( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightPipelineConfig const & config
		, LightPassResult const & lpResult
		, ShadowMapResult const & smResult
		, std::vector< LightRenderPass > const & renderPasses )
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
		, m_descriptorPool{ m_descriptorLayout->createPool( 300u ) }
		, m_pipeline{ pass, context, graph, m_config, m_renderPasses, m_stages, *m_descriptorLayout }
		, m_vertexBuffer{ doCreateVertexBuffer() }
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
			VkBuffer vertexBuffer = m_vertexBuffer.getVertexBuffer();
			VkDeviceSize offset{ m_vertexBuffer.getVertexOffset() };
			VkDescriptorSet baseDS{};
			auto & renderPass = m_renderPasses[index];
			{
				auto beginRenderPass = makeVkStruct< VkRenderPassBeginInfo >( *renderPass.renderPass
					, *renderPass.framebuffer
					, VkRect2D{ {}, renderPass.framebuffer->getDimensions() }
					, uint32_t( renderPass.clearValues.size() )
					, renderPass.clearValues.data() );
				m_context.vkCmdBeginRenderPass( commandBuffer
					, &beginRenderPass
					, VK_SUBPASS_CONTENTS_INLINE );
				auto pipeline = m_pipeline.getPipeline( index );
				m_context.vkCmdBindPipeline( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, pipeline );
				baseDS = m_pipeline.getDescriptorSet();
				m_context.vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, m_pipeline.getPipelineLayout()
					, 0u
					, 1u
					, &baseDS
					, 0u
					, nullptr );
				auto & descriptors = m_enabledLights[0];
				VkDescriptorSet specDS = *descriptors->descriptorSet;
				m_context.vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, m_pipeline.getPipelineLayout()
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

				for ( auto & attach : renderPass.attaches )
				{
					context.setLayoutState( attach.view
						, { attach.output
							, crg::getAccessMask( attach.output )
							, crg::getStageMask( attach.output ) } );
				}
			}
			index = 1u;

			if ( m_enabledLights.size() > 1u )
			{
				auto & renderPasses = m_renderPasses[index];

				auto beginRenderPass = makeVkStruct< VkRenderPassBeginInfo >( *renderPasses.renderPass
					, *renderPasses.framebuffer
					, VkRect2D{ {}, renderPasses.framebuffer->getDimensions() }
					, uint32_t( renderPasses.clearValues.size() )
					, renderPasses.clearValues.data() );

				for ( size_t i = 1u; i < m_enabledLights.size(); ++i )
				{
					auto & descriptors = m_enabledLights[i];
					VkDescriptorSet specDS = *descriptors->descriptorSet;
					m_context.vkCmdBeginRenderPass( commandBuffer
						, &beginRenderPass
						, VK_SUBPASS_CONTENTS_INLINE );
					auto pipeline = m_pipeline.getPipeline( index );
					m_context.vkCmdBindPipeline( commandBuffer
						, VK_PIPELINE_BIND_POINT_GRAPHICS
						, pipeline );
					m_context.vkCmdBindDescriptorSets( commandBuffer
						, VK_PIPELINE_BIND_POINT_GRAPHICS
						, m_pipeline.getPipelineLayout()
						, 0u
						, 1u
						, &baseDS
						, 0u
						, nullptr );
					m_context.vkCmdBindDescriptorSets( commandBuffer
						, VK_PIPELINE_BIND_POINT_GRAPHICS
						, m_pipeline.getPipelineLayout()
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

					for ( auto & attach : renderPass.attaches )
					{
						context.setLayoutState( attach.view
							, { attach.output
								, crg::getAccessMask( attach.output )
								, crg::getStageMask( attach.output ) } );
					}
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

		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eSmNormalLinear )
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
				result.modelMatrixUbo = m_device.uboPools->getBuffer< ModelBufferConfiguration >( 0u );
				writes.emplace_back( result.modelMatrixUbo.getDescriptorWrite( uint32_t( LightPassLgtIdx::eModelMatrix ) ) );
			}

			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmNormalLinear )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { m_device.renderSystem.getEngine()->getDefaultSampler().lock()->getSampler()
					, m_smResult[SmTexture::eNormalLinear].wholeView
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
			auto scale = doCalcPointLightBSphere( *light.getPointLight()
				, float( farZ - castor::point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
			model = castor::matrix::setTransform( model
				, lightPos
				, castor::Point3f{ scale, scale, scale }
				, castor::Quaternion::identity() );
		}
		else
		{
			auto scale = doCalcSpotLightBCone( *light.getSpotLight()
				, float( farZ - castor::point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
			model = castor::matrix::setTransform( model
				, lightPos
				, castor::Point3f{ scale[0], scale[0], scale[1] }
				, light.getParent()->getDerivedOrientation() );
		}

		return model;
	}

	ObjectBufferOffset LightsPipeline::doCreateVertexBuffer()
	{
		ObjectBufferOffset result;

		if ( m_config.lightType == LightType::eDirectional )
		{
			m_count = 6u;
			result = m_device.vertexPools->getBuffer< float >( 12u );
			auto buffer = result.getVertexData< float >();
			float data[] =
			{
				-1.0f, -1.0f,
				-1.0f, +1.0f,
				+1.0f, -1.0f,
				+1.0f, -1.0f,
				-1.0f, +1.0f,
				+1.0f, +1.0f,
			};
			std::memcpy( buffer.data(), data, sizeof( data ) );
			result.vtxBuffer->markDirty( result.getVertexOffset()
				, result.getVertexSize()
				, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}
		else
		{
			castor::Point3fArray data;

			if ( m_config.lightType == LightType::ePoint )
			{
				data = PointLight::generateVertices();
			}
			else
			{
				data = SpotLight::generateVertices();
			}

			m_count = uint32_t( data.size() );
			result = m_device.vertexPools->getBuffer< float >( uint32_t( data.size() * 3u ) );
			auto buffer = result.getVertexData< float >();
			std::memcpy( buffer.data(), data.data()->constPtr(), result.getVertexSize() );
			result.vtxBuffer->markDirty( result.getVertexOffset()
				, result.getVertexSize()
				, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}

		return result;
	}

	//*********************************************************************************************

	RunnableLightingPass::RunnableLightingPass( LightingPass const & lightingPass
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, Scene const & scene
		, LightPassResult const & lpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this](){ doInitialise(); }
				, GetSemaphoreWaitFlagsCallback( [](){ return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback([&lightingPass](){ return lightingPass.isEnabled(); } ) }
			, crg::ru::Config{ 1u, true }
				.implicitAction( lpResult[LpTexture::eDiffuse].targetViewId
					, crg::RecordContext::clearAttachment( lpResult[LpTexture::eDiffuse].targetViewId, getClearValue( LpTexture::eDiffuse ) ) )
				.implicitAction( lpResult[LpTexture::eSpecular].targetViewId
					, crg::RecordContext::clearAttachment( lpResult[LpTexture::eSpecular].targetViewId, getClearValue( LpTexture::eSpecular ) ) ) }
		, m_device{ device }
		, m_scene{ scene }
		, m_lpResult{ lpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
	{
	}

	void RunnableLightingPass::clear()
	{
		for ( auto & pipeline : m_pipelines )
		{
			pipeline.second->clear();
		}
	}

	void RunnableLightingPass::enableLight( Camera const & camera
		, Light const & light )
	{
		auto & pipeline = doFindPipeline( light );
		pipeline.addLight( camera, light );
	}

	void RunnableLightingPass::disableLight( Camera const & camera
		, Light const & light )
	{
		auto & pipeline = doFindPipeline( light );
		pipeline.removeLight( camera, light );
	}

	void RunnableLightingPass::resetCommandBuffer()
	{
		crg::RunnablePass::resetCommandBuffer();
		crg::RunnablePass::reRecordCurrent();
	}

	bool RunnableLightingPass::hasEnabledLights()const
	{
		return !m_pipelines.empty();
	}

	void RunnableLightingPass::doInitialise()
	{
		if ( m_renderPasses.empty() )
		{
			m_renderPasses.emplace_back( doCreateRenderPass( false, m_lpResult ) );
			m_renderPasses.emplace_back( doCreateRenderPass( true, m_lpResult ) );
		}
	}

	void RunnableLightingPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		uint32_t pipelineIndex = 0u;

		for ( auto & pipeline : m_pipelines )
		{
			pipeline.second->recordInto( context, commandBuffer, pipelineIndex );
		}
	}

	LightRenderPass RunnableLightingPass::doCreateRenderPass( bool blend
		, LightPassResult const & lpResult )
	{
		castor::String name = blend
			? castor::String{ cuT( "Blend" ) }
			: castor::String{ cuT( "First" ) };
		std::array< VkImageLayout, 3u > layouts{ ( blend ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
			, ( blend ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED )
			, ( blend ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED ) };
		VkAttachmentLoadOp loadOp = blend
			? VK_ATTACHMENT_LOAD_OP_LOAD
			: VK_ATTACHMENT_LOAD_OP_CLEAR;

		ashes::VkAttachmentDescriptionArray attaches{
			{ 0u
				, lpResult[LpTexture::eDepth].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, VK_ATTACHMENT_LOAD_OP_LOAD
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[0u]
				, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eDiffuse].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[1u]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eSpecular].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layouts[2u]
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } };
		ashes::VkAttachmentReferenceArray references{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };

		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription{ 0u
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, {}
			, references
			, {}
			, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, {} } );

		ashes::VkSubpassDependencyArray dependencies{
			{ VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
			, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
		ashes::RenderPassCreateInfo rpCreateInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		LightRenderPass result{};
		result.renderPass = m_device->createRenderPass( "LightPass" + name
			, std::move( rpCreateInfo ) );

		std::vector< VkImageView > viewAttaches{ lpResult[LpTexture::eDepth].targetView
			, lpResult[LpTexture::eDiffuse].targetView
			, lpResult[LpTexture::eSpecular].targetView };
		auto fbCreateInfo = makeVkStruct< VkFramebufferCreateInfo >( 0u
			, VkRenderPass{}
			, uint32_t{}
			, nullptr
			, uint32_t{}
			, uint32_t{}
			, uint32_t{} );
		fbCreateInfo.renderPass = *result.renderPass;
		fbCreateInfo.attachmentCount = uint32_t( viewAttaches.size() );
		fbCreateInfo.pAttachments = viewAttaches.data();
		fbCreateInfo.width = lpResult[LpTexture::eDiffuse].getExtent().width;
		fbCreateInfo.height = lpResult[LpTexture::eDiffuse].getExtent().height;
		fbCreateInfo.layers = 1u;
		result.framebuffer = result.renderPass->createFrameBuffer( "LightPass" + name
			, std::move( fbCreateInfo ) );

		result.clearValues.push_back( getClearValue( LpTexture::eDepth ) );
		result.clearValues.push_back( getClearValue( LpTexture::eDiffuse ) );
		result.clearValues.push_back( getClearValue( LpTexture::eSpecular ) );

		result.attaches.push_back( { lpResult[LpTexture::eDepth].targetViewId
			, layouts[0]
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } );
		result.attaches.push_back( { lpResult[LpTexture::eDiffuse].targetViewId
			, layouts[1]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		result.attaches.push_back( { lpResult[LpTexture::eSpecular].targetViewId
			, layouts[2]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );

		return result;
	}

	LightsPipeline & RunnableLightingPass::doFindPipeline( Light const & light )
	{
		LightPipelineConfig config{ m_scene.getPassesType()
			, m_scene.getFlags()
			, light };
		auto hash = config.makeHash();
		auto it = m_pipelines.find( hash );

		if ( it == m_pipelines.end() )
		{
			it = m_pipelines.emplace( hash
				, std::make_unique< LightsPipeline >( m_pass
					, m_context
					, m_graph
					, m_device
					, config
					, m_lpResult
					, ( config.lightType == LightType::eDirectional
						? m_smDirectionalResult
						: ( config.lightType == LightType::ePoint
							? m_smPointResult
							: m_smSpotResult ) )
					, m_renderPasses ) ).first;
		}

		return *it->second;
	}

	//*********************************************************************************************

	LightingPass::LightingPass( crg::FramePassGroup & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, ProgressBar * progress
		, castor::Size const & size
		, Scene & scene
		, Texture const & depth
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightPassResult const & lpResult
		, SceneUbo const & sceneUbo
		, GpInfoUbo const & gpInfoUbo )
		: m_device{ device }
		, m_depth{ depth }
		, m_gpResult{ gpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
		, m_lpResult{ lpResult }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_group{ graph.createPassGroup( "DirectLighting" ) }
		, m_size{ size }
	{
		previousPass = &doCreateDepthBlitPass( m_group
			, *previousPass
			, progress );
		previousPass = &doCreateLightingPass( m_group
			, *previousPass
			, scene
			, progress );
	}

	void LightingPass::update( CpuUpdater & updater )
	{
		if ( m_lightPass )
		{
			auto & scene = *updater.camera->getScene();
			auto & cache = scene.getLightCache();

			m_lightPass->clear();

			if ( !cache.isEmpty() )
			{
				doUpdateLightPasses( updater, LightType::eDirectional );
				doUpdateLightPasses( updater, LightType::ePoint );
				doUpdateLightPasses( updater, LightType::eSpot );
			}

			m_lightPass->resetCommandBuffer();
		}
	}

	void LightingPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( "Light Diffuse"
			, m_lpResult[LpTexture::eDiffuse]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Specular"
			, m_lpResult[LpTexture::eSpecular]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
	}

	void LightingPass::doUpdateLightPasses( CpuUpdater & updater
		, LightType lightType )
	{
		auto & camera = *updater.camera;
		auto & scene = *updater.camera->getScene();
		auto & cache = scene.getLightCache();

		for ( auto & light : cache.getLights( lightType ) )
		{
			if ( light->getLightType() == LightType::eDirectional
				|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
			{
				m_lightPass->enableLight( camera, *light );
			}
			else
			{
				m_lightPass->disableLight( camera, *light );
			}
		}
	}

	crg::FramePass const & LightingPass::doCreateDepthBlitPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating depth blit pass" );
		auto & engine = *m_device.renderSystem.getEngine();
		auto & pass = graph.createPass( "DepthBlit"
			, [this, progress, &engine]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising depth blit pass" );
				auto result = std::make_unique< crg::ImageCopy >( framePass
					, context
					, graph
					, makeExtent3D( m_size )
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [this](){ return isEnabled(); } ) );
				engine.registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		pass.addTransferInputView( m_depth.wholeViewId );
		pass.addTransferOutputView( m_lpResult[LpTexture::eDepth].wholeViewId );
		return pass;
	}

	crg::FramePass const & LightingPass::doCreateLightingPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, Scene const & scene
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating lighting pass" );
		auto & engine = *m_device.renderSystem.getEngine();
		auto & pass = graph.createPass( "Lighting"
			, [this, progress, &engine, &scene]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising lighting pass" );
				auto result = std::make_unique< RunnableLightingPass >( *this
					, framePass
					, context
					, runnableGraph
					, m_device
					, scene
					, m_lpResult
					, m_smDirectionalResult
					, m_smPointResult
					, m_smSpotResult );
				engine.registerTimer( framePass.getFullName()
					, result->getTimer() );
				m_lightPass = result.get();
				return result;
			} );
		pass.addDependency( previousPass );
		engine.getMaterialCache().getPassBuffer().createPassBinding( pass
			, uint32_t( LightPassIdx::eMaterials ) );
		engine.getMaterialCache().getSssProfileBuffer().createPassBinding( pass
			, uint32_t( LightPassIdx::eSssProfiles ) );
		m_gpInfoUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eGpInfo ) );
		m_sceneUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eScene ) );
		pass.addSampledView( m_gpResult[DsTexture::eData0].sampledViewId
			, uint32_t( LightPassIdx::eData0 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData1].sampledViewId
			, uint32_t( LightPassIdx::eData1 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData2].sampledViewId
			, uint32_t( LightPassIdx::eData2 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData3].sampledViewId
			, uint32_t( LightPassIdx::eData3 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData4].sampledViewId
			, uint32_t( LightPassIdx::eData4 ) );

		pass.addInOutDepthStencilView( m_lpResult[LpTexture::eDepth].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eDiffuse].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eSpecular].targetViewId );
		return pass;
	}
}
