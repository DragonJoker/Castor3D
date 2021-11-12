#include "Castor3D/Render/Passes/StencilPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FrameGraph.hpp>

CU_ImplementCUSmartPtr( castor3d, StencilPass )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ShaderPtr doGetVertexShader( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			UBO_MATRIX( writer, 0u, 0u );
			UBO_MODEL( writer, 1u, 0u );
			auto vertex = writer.declInput< Vec3 >( "position", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = c3d_matrixData.worldToCurProj( c3d_modelData.modelToWorld( vec4( vertex, 1.0_f ) ) );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	StencilPass::StencilPass( Engine const & engine
		, String const & prefix
		, crg::ImageViewId depthView
		, MatrixUbo & matrixUbo
		, UniformBufferT< ModelUboConfiguration > const & modelUbo )
		: m_engine{ engine }
		, m_prefix{ prefix }
		, m_depthView{ depthView }
		, m_matrixUbo{ matrixUbo }
		, m_modelUbo{ modelUbo }
	{
	}

	void StencilPass::initialise( RenderDevice const & device
		, ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
		, ashes::VertexBufferBase & vbo )
	{
		m_vbo = &vbo;
		auto & renderSystem = *m_engine.getRenderSystem();
		VkExtent2D size{ m_depthView.data->image.data->info.extent.width
			, m_depthView.data->image.data->info.extent.height };
		auto name = m_prefix + "StencilPass";

		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings
		{
			makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
			makeDescriptorSetLayoutBinding( 1u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
		};
		m_descriptorLayout = device->createDescriptorSetLayout( name
			, std::move( setLayoutBindings ) );
		m_pipelineLayout = device->createPipelineLayout( name 
			, *m_descriptorLayout );

		m_descriptorPool = m_descriptorLayout->createPool( name, 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet( name );
		m_matrixUbo.createSizedBinding( *m_descriptorSet
			, m_descriptorLayout->getBinding( 0u ) );
		m_descriptorSet->createSizedBinding( m_descriptorLayout->getBinding( 1u )
			, m_modelUbo.getBuffer() );
		m_descriptorSet->update();

		m_program =
		{
			makeShaderState( device
				, ShaderModule{ VK_SHADER_STAGE_VERTEX_BIT, name, doGetVertexShader( renderSystem ) } ),
		};

		ashes::PipelineDepthStencilStateCreateInfo dsstate;
		dsstate->depthTestEnable = VK_TRUE;
		dsstate->depthWriteEnable = VK_FALSE;
		dsstate->stencilTestEnable = VK_TRUE;
		dsstate->back.compareMask = 0u;
		dsstate->back.reference = 0u;
		dsstate->back.compareOp = VK_COMPARE_OP_ALWAYS;
		dsstate->back.failOp = VK_STENCIL_OP_KEEP;
		dsstate->back.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_WRAP;
		dsstate->back.passOp = VK_STENCIL_OP_KEEP;
		dsstate->front.compareMask = 0u;
		dsstate->front.reference = 0u;
		dsstate->front.compareOp = VK_COMPARE_OP_ALWAYS;
		dsstate->front.failOp = VK_STENCIL_OP_KEEP;
		dsstate->front.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_WRAP;
		dsstate->front.passOp = VK_STENCIL_OP_KEEP;
		
		m_pipeline = device->createPipeline( name
			, ashes::GraphicsPipelineCreateInfo
			{
				0u,
				m_program,
				vertexLayout,
				ashes::PipelineInputAssemblyStateCreateInfo{},
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo
				{
					0u,
					{ 1u, VkViewport{ 0.0f, 0.0f, float( size.width ), float( size.height ), 0.0f, 1.0f } },
					{ 1u, VkRect2D{ 0, 0, size.width, size.height } }
				},
				ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
				ashes::PipelineMultisampleStateCreateInfo{},
				std::move( dsstate ),
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::nullopt,
				*m_pipelineLayout,
				*m_renderPass,
			} );

		// TODO CRG
		//m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( name, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		//m_finished = device->createSemaphore( name );

		//m_commandBuffer->begin();
		//m_commandBuffer->beginDebugBlock(
		//	{
		//		"Deferred - Stencil",
		//		makeFloatArray( m_engine.getNextRainbowColour() ),
		//	} );
		//m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//	, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
		//	, m_depthView.makeDepthStencilAttachment( VK_IMAGE_LAYOUT_UNDEFINED ) );
		//m_commandBuffer->beginRenderPass( *m_renderPass
		//	, *m_frameBuffer
		//	, { defaultClearDepthStencil }
		//	, VK_SUBPASS_CONTENTS_INLINE );
		//m_commandBuffer->bindPipeline( *m_pipeline );
		//m_commandBuffer->bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		//m_commandBuffer->bindVertexBuffer( 0u, vbo.getBuffer(), 0u );
		//m_commandBuffer->draw( uint32_t( vbo.getSize() / vertexLayout.vertexBindingDescriptions[0].stride ) );
		//m_commandBuffer->endRenderPass();
		//m_commandBuffer->endDebugBlock();
		//m_commandBuffer->end();
	}

	void StencilPass::cleanup()
	{
		m_finished.reset();
		m_commandBuffer.reset();
		m_pipeline.reset();
		m_program.clear();
		m_pipelineLayout.reset();
		m_frameBuffer.reset();
		m_renderPass.reset();
		m_descriptorSet.reset();
		m_descriptorPool.reset();
		m_descriptorLayout.reset();
	}

	ashes::Semaphore const & StencilPass::render( QueueData const & queueData
		, ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;

		queueData.queue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	//*********************************************************************************************
}
