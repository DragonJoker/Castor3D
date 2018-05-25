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
		, Parameters const & CU_PARAM_UNUSED( parameters )
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

	bool PostEffect::writeInto( castor::TextFile & p_file )
	{
		return doWriteInto( p_file );
	}

	bool PostEffect::initialise( TextureLayout const & texture )
	{
		auto & device = *getRenderSystem()->getCurrentDevice();
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
		ENSURE( m_result != nullptr );
		return result;
	}

	void PostEffect::cleanup()
	{
		m_commands.clear();
		doCleanup();
		m_timer.reset();
	}

	void PostEffect::start()
	{
		m_timer->start();
		m_currentPass = 0u;
	}

	void PostEffect::notifyPassRender()
	{
		m_timer->notifyPassRender( m_currentPass++ );
	}

	void PostEffect::stop()
	{
		m_timer->stop();
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
