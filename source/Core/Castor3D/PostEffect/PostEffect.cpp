#include "Castor3D/PostEffect/PostEffect.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

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

	void PostEffect::doCopyResultToTarget( ashes::ImageView const & result
		, ashes::CommandBuffer & commandBuffer )
	{
		// Put result image in transfer source layout.
		commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, result.makeTransferSource( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		// Put target image in transfer destination layout.
		commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_target->getDefaultView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Copy result to target.
		commandBuffer.copyImage( result
			, m_target->getDefaultView() );
		// Put target image in fragment shader input layout.
		commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_target->getDefaultView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
	}
}
