#include "StencilPass.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"

#include <Ashes/Buffer/VertexBuffer.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::RenderPassPtr doCreateRenderPass( Engine const & engine
			, ashes::TextureView const & depthView )
		{
			auto & device = getCurrentDevice( engine );

			ashes::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = depthView.getFormat();
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eClear;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].depthStencilAttachment = { 0u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };

			return device.createRenderPass( renderPass );
		}

		ShaderPtr doGetVertexShader( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			UBO_MATRIX( writer, 0u, 0u );
			UBO_MODEL_MATRIX( writer, 1u, 0u );
			auto vertex = writer.declInput< Vec3 >( cuT( "position" ), 0u );

			// Shader outputs
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				out.gl_out.gl_Position = c3d_projection * c3d_curView * c3d_curMtxModel * vec4( vertex, 1.0 );
			} );

			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	StencilPass::StencilPass( Engine const & engine
		, ashes::TextureView const & depthView
		, MatrixUbo & matrixUbo
		, ModelMatrixUbo & modelMatrixUbo )
		: m_engine{ engine }
		, m_depthView{ depthView }
		, m_matrixUbo{ matrixUbo }
		, m_modelMatrixUbo{ modelMatrixUbo }
	{
	}

	void StencilPass::initialise( ashes::VertexLayout const & vertexLayout
		, ashes::VertexBufferBase & vbo )
	{
		m_vbo = &vbo;
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		ashes::Extent2D size{ m_depthView.getTexture().getDimensions().width, m_depthView.getTexture().getDimensions().height };

		ashes::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },
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
		ashes::FrameBufferAttachmentArray attaches
		{
			{ *m_renderPass->getAttachments().begin(), m_depthView }
		};
		m_frameBuffer = m_renderPass->createFrameBuffer( size, std::move( attaches ) );

		m_program =
		{
			{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
		};
		m_program[0].module->loadShader( renderSystem.compileShader( { ashes::ShaderStageFlag::eVertex, "StencilPass", doGetVertexShader( renderSystem ) } ) );

		ashes::DepthStencilState dsstate;
		dsstate.depthTestEnable = true;
		dsstate.depthWriteEnable = false;
		dsstate.stencilTestEnable = true;
		dsstate.back.compareMask = 0u;
		dsstate.back.reference = 0u;
		dsstate.back.compareOp = ashes::CompareOp::eAlways;
		dsstate.back.failOp = ashes::StencilOp::eKeep;
		dsstate.back.depthFailOp = ashes::StencilOp::eIncrementAndWrap;
		dsstate.back.passOp = ashes::StencilOp::eKeep;
		dsstate.front.compareMask = 0u;
		dsstate.front.reference = 0u;
		dsstate.front.compareOp = ashes::CompareOp::eAlways;
		dsstate.front.failOp = ashes::StencilOp::eKeep;
		dsstate.front.depthFailOp = ashes::StencilOp::eDecrementAndWrap;
		dsstate.front.passOp = ashes::StencilOp::eKeep;

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			m_program,
			*m_renderPass,
			ashes::VertexInputState::create( vertexLayout ),
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{ 0u, false, false, ashes::PolygonMode::eFill, ashes::CullModeFlag::eNone },
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{},
			dsstate,
			ashes::nullopt,
			ashes::Viewport{ size.width, size.height, 0, 0 },
			ashes::Scissor{ 0, 0, size.width, size.height },
		} );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );
		m_finished = device.createSemaphore();

		m_commandBuffer->begin();
		m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eFragmentShader
			, ashes::PipelineStageFlag::eEarlyFragmentTests
			, m_depthView.makeDepthStencilAttachment( ashes::ImageLayout::eUndefined, 0u ) );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { ashes::DepthStencilClearValue{ 1.0, 0 } }
			, ashes::SubpassContents::eInline );
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

	ashes::Semaphore const & StencilPass::render( ashes::Semaphore const & toWait )
	{
		auto & device = getCurrentDevice( m_engine );
		auto * result = &toWait;

		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	//*********************************************************************************************
}
