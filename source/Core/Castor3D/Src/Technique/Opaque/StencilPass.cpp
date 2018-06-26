#include "StencilPass.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		renderer::RenderPassPtr doCreateRenderPass( Engine const & engine
			, renderer::TextureView const & depthView )
		{
			auto & device = getCurrentDevice( engine );

			renderer::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = depthView.getFormat();
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };

			return device.createRenderPass( renderPass );
		}

		glsl::Shader doGetVertexShader( RenderSystem & renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer, 0u, 0u );
			UBO_MODEL_MATRIX( writer, 1u, 0u );
			auto vertex = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );

			// Shader outputs
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				out.gl_Position() = c3d_projection * c3d_curView * c3d_curMtxModel * vec4( vertex, 1.0 );
			} );

			return writer.finalise();
		}
	}

	//*********************************************************************************************

	StencilPass::StencilPass( Engine const & engine
		, renderer::TextureView const & depthView
		, MatrixUbo & matrixUbo
		, ModelMatrixUbo & modelMatrixUbo )
		: m_engine{ engine }
		, m_depthView{ depthView }
		, m_matrixUbo{ matrixUbo }
		, m_modelMatrixUbo{ modelMatrixUbo }
	{
	}

	void StencilPass::initialise( renderer::VertexLayout const & vertexLayout
		, renderer::VertexBufferBase & vbo )
	{
		m_vbo = &vbo;
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		renderer::Extent2D size{ m_depthView.getTexture().getDimensions().width, m_depthView.getTexture().getDimensions().height };

		renderer::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );

		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet();
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
			, m_matrixUbo.getUbo() );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
			, m_modelMatrixUbo.getUbo() );
		m_descriptorSet->update();

		m_renderPass = doCreateRenderPass( m_engine, m_depthView );
		renderer::FrameBufferAttachmentArray attaches
		{
			{ *m_renderPass->getAttachments().begin(), m_depthView }
		};
		m_frameBuffer = m_renderPass->createFrameBuffer( size, std::move( attaches ) );

		m_program =
		{
			{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
		};
		m_program[0].module->loadShader( doGetVertexShader( renderSystem ).getSource() );

		renderer::DepthStencilState dsstate;
		dsstate.depthTestEnable = true;
		dsstate.depthWriteEnable = false;
		dsstate.stencilTestEnable = true;
		dsstate.back.compareMask = 0u;
		dsstate.back.reference = 0u;
		dsstate.back.compareOp = renderer::CompareOp::eAlways;
		dsstate.back.failOp = renderer::StencilOp::eKeep;
		dsstate.back.depthFailOp = renderer::StencilOp::eIncrementAndWrap;
		dsstate.back.passOp = renderer::StencilOp::eKeep;
		dsstate.front.compareMask = 0u;
		dsstate.front.reference = 0u;
		dsstate.front.compareOp = renderer::CompareOp::eAlways;
		dsstate.front.failOp = renderer::StencilOp::eKeep;
		dsstate.front.depthFailOp = renderer::StencilOp::eDecrementAndWrap;
		dsstate.front.passOp = renderer::StencilOp::eKeep;

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			m_program,
			*m_renderPass,
			renderer::VertexInputState::create( vertexLayout ),
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{},
			dsstate,
			std::nullopt,
			renderer::Viewport{ size.width, size.height, 0, 0 },
			renderer::Scissor{ 0, 0, size.width, size.height },
		} );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );
		m_finished = device.createSemaphore();

		m_commandBuffer->begin();
		m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eFragmentShader
			, renderer::PipelineStageFlag::eEarlyFragmentTests
			, m_depthView.makeDepthStencilAttachment( renderer::ImageLayout::eUndefined, 0u ) );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { renderer::DepthStencilClearValue{ 1.0, 0 } }
			, renderer::SubpassContents::eInline );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		m_commandBuffer->bindVertexBuffer( 0u, vbo.getBuffer(), 0u );
		m_commandBuffer->draw( vbo.getSize() / vertexLayout.getStride() );
		m_commandBuffer->endRenderPass();
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

	renderer::Semaphore const & StencilPass::render( renderer::Semaphore const & toWait )
	{
		auto & device = getCurrentDevice( m_engine );
		auto * result = &toWait;

		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	//*********************************************************************************************
}
