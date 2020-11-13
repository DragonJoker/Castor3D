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
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, StencilPass )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::RenderPassPtr doCreateRenderPass( castor::String const & name
			, RenderDevice const & device
			, ashes::ImageView const & depthView )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					depthView->format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{},
					{},
					VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies;
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( name
				, std::move( createInfo ) );
			return result;
		}

		ShaderPtr doGetVertexShader( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			UBO_MATRIX( writer, 0u, 0u );
			UBO_MODEL_MATRIX( writer, 1u, 0u );
			auto vertex = writer.declInput< Vec3 >( "position", 0u );

			// Shader outputs
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.vtx.position = c3d_projection * c3d_curView * c3d_curMtxModel * vec4( vertex, 1.0_f );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	StencilPass::StencilPass( Engine const & engine
		, String const & prefix
		, ashes::ImageView const & depthView
		, MatrixUbo & matrixUbo
		, UniformBufferOffsetT< ModelMatrixUboConfiguration > const & modelMatrixUbo )
		: m_engine{ engine }
		, m_prefix{ prefix }
		, m_depthView{ depthView }
		, m_matrixUbo{ matrixUbo }
		, m_modelMatrixUbo{ modelMatrixUbo }
	{
	}

	void StencilPass::initialise( RenderDevice const & device
		, ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
		, ashes::VertexBufferBase & vbo )
	{
		m_vbo = &vbo;
		auto & renderSystem = *m_engine.getRenderSystem();
		VkExtent2D size{ m_depthView.image->getDimensions().width, m_depthView.image->getDimensions().height };
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
		m_modelMatrixUbo.createSizedBinding( *m_descriptorSet
			, m_descriptorLayout->getBinding( 1u ) );
		m_descriptorSet->update();

		m_renderPass = doCreateRenderPass( name, device, m_depthView );
		ashes::ImageViewCRefArray attaches
		{
			m_depthView
		};
		m_frameBuffer = m_renderPass->createFrameBuffer( name, size, std::move( attaches ) );

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

		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( name, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		m_finished = device->createSemaphore( name );

		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
		m_commandBuffer->beginDebugBlock(
			{
				"Deferred - Stencil",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_depthView.makeDepthStencilAttachment( VK_IMAGE_LAYOUT_UNDEFINED ) );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { defaultClearDepthStencil }
			, VK_SUBPASS_CONTENTS_INLINE );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		m_commandBuffer->bindVertexBuffer( 0u, vbo.getBuffer(), 0u );
		m_commandBuffer->draw( uint32_t( vbo.getSize() / vertexLayout.vertexBindingDescriptions[0].stride ) );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
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

	ashes::Semaphore const & StencilPass::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	//*********************************************************************************************
}
