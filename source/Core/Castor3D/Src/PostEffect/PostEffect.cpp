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
		, castor::String const & fullName
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & CU_UnusedParam( parameters )
		, bool postToneMapping )
		: OwnedBy< RenderSystem >{ renderSystem }
		, Named{ name }
		, m_fullName{ fullName }
		, m_renderTarget{ renderTarget }
		, m_postToneMapping{ postToneMapping }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	bool PostEffect::writeInto( castor::TextFile & file, castor::String const & tabs )
	{
		return doWriteInto( file, tabs );
	}

	bool PostEffect::initialise( TextureLayout const & texture )
	{
		m_target = &texture;
		auto name = m_fullName;
		name = castor::string::replace( name, cuT( "PostEffect" ), castor::String{} );
		name = castor::string::replace( name, cuT( "Post Effect" ), castor::String{} );
		m_timer = std::make_unique< RenderPassTimer >( *getRenderSystem()->getEngine()
			, ( m_postToneMapping
				? String{ cuT( "sRGB PostEffect" ) }
				: String{ cuT( "HDR PostEffect" ) } )
			, name
			, m_passesCount );
		auto result = doInitialise( *m_timer );
		CU_Ensure( m_result != nullptr );
		return result;
	}

	void PostEffect::cleanup()
	{
		m_commands.clear();
		doCleanup();
		m_timer.reset();
	}

	RenderPassTimerBlock PostEffect::start()
	{
		m_currentPass = 0u;
		return m_timer->start();
	}

	void PostEffect::notifyPassRender()
	{
		m_timer->notifyPassRender( m_currentPass++ );
	}

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
	}

	void PostEffect::doCopyResultToTarget( ashes::TextureView const & result
		, ashes::CommandBuffer & commandBuffer )
	{
		// Put result image in transfer source layout.
		commandBuffer.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eTransfer
			, result.makeTransferSource( ashes::ImageLayout::eColourAttachmentOptimal
				, ashes::AccessFlag::eColourAttachmentWrite ) );
		// Put target image in transfer destination layout.
		commandBuffer.memoryBarrier( ashes::PipelineStageFlag::eFragmentShader
			, ashes::PipelineStageFlag::eTransfer
			, m_target->getDefaultView().makeTransferDestination( ashes::ImageLayout::eUndefined, 0u ) );
		// Copy result to target.
		commandBuffer.copyImage( result
			, m_target->getDefaultView() );
		// Put target image in fragment shader input layout.
		commandBuffer.memoryBarrier( ashes::PipelineStageFlag::eTransfer
			, ashes::PipelineStageFlag::eFragmentShader
			, m_target->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eTransferDstOptimal
				, ashes::AccessFlag::eTransferWrite ) );
	}
}
