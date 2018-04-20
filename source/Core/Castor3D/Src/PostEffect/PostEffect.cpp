#include "PostEffect.hpp"

#include "Engine.hpp"

#include "Render/RenderTarget.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

using namespace castor;
namespace castor3d
{
	PostEffect::PostEffect( String const & name
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & CU_PARAM_UNUSED( parameters )
		, bool postToneMapping )
		: OwnedBy< RenderSystem >{ renderSystem }
		, Named{ name }
		, m_renderTarget{ renderTarget }
		, m_postToneMapping{ postToneMapping }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	bool PostEffect::writeInto( castor::TextFile & p_file )
	{
		return doWriteInto( p_file );
	}

	bool PostEffect::initialise( TextureLayout const & texture
		, RenderPassTimer const & timer )
	{
		auto & device = *getRenderSystem()->getCurrentDevice();
		m_signalFinished = device.createSemaphore();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_target = &texture;
		auto result = doInitialise( timer );
		ENSURE( m_result != nullptr );
		return result;
	}

	void PostEffect::cleanup()
	{
		doCleanup();
		m_commandBuffer.reset();
		m_signalFinished.reset();
	}

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
	}

	void PostEffect::doCopyResultToTarget( renderer::TextureView const & result
		, renderer::CommandBuffer & commandBuffer )
	{
		// Put result image in transfer source layout.
		commandBuffer.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::PipelineStageFlag::eTransfer
			, result.makeTransferSource( renderer::ImageLayout::eColourAttachmentOptimal
				, renderer::AccessFlag::eColourAttachmentWrite ) );
		// Put target image in transfer destination layout.
		commandBuffer.memoryBarrier( renderer::PipelineStageFlag::eFragmentShader
			, renderer::PipelineStageFlag::eTransfer
			, m_target->getDefaultView().makeTransferDestination( renderer::ImageLayout::eUndefined, 0u ) );
		// Copy result to target.
		commandBuffer.copyImage( result
			, m_target->getDefaultView() );
		// Put target image in fragment shader input layout.
		commandBuffer.memoryBarrier( renderer::PipelineStageFlag::eTransfer
			, renderer::PipelineStageFlag::eFragmentShader
			, m_target->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eTransferDstOptimal
				, renderer::AccessFlag::eTransferWrite ) );
	}
}
