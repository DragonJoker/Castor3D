#include "Castor3D/Render/Opaque/Lighting/ClusteredLightsPipeline.hpp"

#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"

namespace castor3d
{
	namespace clslgtpass
	{
		static LightPipelineConfig updateConfig( LightPipelineConfig config )
		{
			config.clustered = true;
			return config;
		}
	}

	//*********************************************************************************************

	ClusteredLightsPipeline::ClusteredLightsPipeline( Scene const & scene
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, FrustumClusters const & frustumClusters
		, LightPipelineConfig const & config
		, LightPassResult const & lpResult
		, LightRenderPassArray const & renderPasses
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, crg::ImageViewIdArray const & targetColourResult )
		: m_context{ context }
		, m_device{ device }
		, m_renderPasses{ renderPasses }
		, m_scene{ scene }
		, m_frustumClusters{ frustumClusters }
		, m_targetColourResult{ targetColourResult }
		, m_cameraUbo{ m_device }
		, m_config{ clslgtpass::updateConfig( config ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, "Vtx" + m_config.getName( *m_scene.getEngine() )
			, LightPass::getVertexShaderSource( LightType::eDirectional ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, "Pxl" + m_config.getName( *m_scene.getEngine() )
			, LightPass::getPixelShaderSource( m_config.lightingModelId
				, m_scene.getBackgroundModelId()
				, m_scene
				, m_scene.getDebugConfig()
				, m_config.sceneFlags
				, makeExtent2D( lpResult[LpTexture::eDiffuse].getExtent() ) ) }
		, m_stages{ makeShaderState( m_device, m_vertexShader )
			, makeShaderState( m_device, m_pixelShader ) }
		, m_descriptorLayout{ doCreateDescriptorLayout( shadowBuffer ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( "ClusteredLights", 1u ) }
		, m_descriptorSet{ doCreateDescriptorSet( graph, shadowMaps, shadowBuffer ) }
		, m_lightPipeline{ pass
			, context
			, graph
			, m_config
			, m_renderPasses
			, m_stages
			, *m_descriptorLayout
			, makeExtent2D( lpResult[LpTexture::eDiffuse].getExtent() ) }
		, m_vertexBuffer{ doCreateVertexBuffer() }
		, m_viewport{ *device.renderSystem.getEngine() }
	{
		m_viewport.setOrtho( -1, 1, -1, 1, -1, 1 );
	}

	void ClusteredLightsPipeline::clear()
	{
		m_viewport.resize( makeSize( getExtent( m_targetColourResult.front() ) ) );
		m_viewport.update();
		m_lightsCount = 0u;
	}

	void ClusteredLightsPipeline::updateCamera( Camera const & camera )
	{
		m_cameraUbo.cpuUpdate( camera
			, camera.getView()
			, m_viewport.getProjection()
			, 0u
			, true );
	}

	void ClusteredLightsPipeline::addLight( Light const & light )
	{
		++m_lightsCount;
	}

	void ClusteredLightsPipeline::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, u32 passIndex
		, u32 & pipelineIndex )
	{
		if ( m_lightsCount == 0u )
		{
			return;
		}

		m_context.vkCmdBeginDebugBlock( commandBuffer
			, { "ClusteredLights" + m_config.getName( *m_scene.getEngine() )
			, m_context.getNextRainbowColour() } );

		auto baseDS = m_lightPipeline.getDescriptorSet();
		auto & renderPass = m_renderPasses[getLightRenderPassIndex( pipelineIndex != 0u, LightType::eMax, true )];
		VkBuffer vertexBuffer{ m_vertexBuffer.getBuffer() };
		VkDeviceSize offset{ m_vertexBuffer.getOffset() };
		VkDescriptorSet specDS = *m_descriptorSet;

		auto beginRenderPass = makeVkStruct< VkRenderPassBeginInfo >( *renderPass.renderPass
			, *renderPass.framebuffers[passIndex].fbo
			, VkRect2D{ {}, renderPass.framebuffers[passIndex].fbo->getDimensions() }
			, u32( renderPass.clearValues.size() )
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
		m_context.vkCmdDraw( commandBuffer
			, m_count
			, 1u
			, 0u
			, 0u );
		m_context.vkCmdEndRenderPass( commandBuffer );

		for ( auto & attach : renderPass.framebuffers[passIndex].attaches )
		{
			if ( attach.view.size() == 1u )
			{
				auto view = attach.view.front();
				context.setLayoutState( resolveView( view, passIndex )
					, { attach.output
					, crg::getAccessMask( attach.output )
					, crg::getStageMask( attach.output ) } );
			}
		}

		m_context.vkCmdEndDebugBlock( commandBuffer );
		pipelineIndex = 1u;
	}

	ashes::DescriptorSetLayoutPtr ClusteredLightsPipeline::doCreateDescriptorLayout( ShadowBuffer const * shadowBuffer )
	{
		PipelineFlags flags{ PassComponentCombine{}, m_config.lightingModelId, {} };
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		u32 index{};
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( index++ // eLights
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( index++ // eMatrix
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );
		RenderNodesPass::addClusteredLightingBindings( m_frustumClusters, setLayoutBindings, index );
		
		if ( shadowBuffer )
		{
			RenderNodesPass::addShadowBindings( m_scene.getFlags(), setLayoutBindings, index );
		}

		RenderNodesPass::addBackgroundBindings( *m_scene.getBackground(), flags, setLayoutBindings, index );

		return m_device->createDescriptorSetLayout( "ClusteredLights"
			, std::move( setLayoutBindings ) );
	}

	ashes::DescriptorSetPtr ClusteredLightsPipeline::doCreateDescriptorSet( crg::RunnableGraph & graph
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		ashes::WriteDescriptorSetArray writes;
		u32 index{};
		PipelineFlags flags{ PassComponentCombine{}, m_config.lightingModelId, {} };

		writes.emplace_back( m_scene.getLightCache().getBinding( index++ ) );
		writes.emplace_back( m_cameraUbo.getDescriptorWrite( index++ ) );
		RenderNodesPass::addClusteredLightingDescriptor( m_frustumClusters, writes, index );

		if ( shadowBuffer )
		{
			RenderNodesPass::addShadowDescriptor( m_device.renderSystem, graph, m_scene.getFlags(), writes, shadowMaps, *shadowBuffer, index );
		}

		RenderNodesPass::addBackgroundDescriptor( *m_scene.getBackground(), flags, writes, m_targetColourResult, index );

		auto result = m_descriptorPool->createDescriptorSet( "ClusteredLights"
			, 1u );
		result->setBindings( writes );
		result->update();
		return result;
	}

	GpuBufferOffsetT< float > ClusteredLightsPipeline::doCreateVertexBuffer()
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
		auto result = m_device.bufferPool->getBuffer< float >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			, 12u
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		auto buffer = result.getData();
		std::memcpy( buffer.data(), data, sizeof( data ) );

		result.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		return result;
	}

	//*********************************************************************************************
}
