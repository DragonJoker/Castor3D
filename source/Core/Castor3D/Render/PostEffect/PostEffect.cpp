#include "Castor3D/Render/PostEffect/PostEffect.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

namespace castor3d
{
	namespace
	{
		castor::String getKindName( PostEffect::Kind kind )
		{
			switch ( kind )
			{
			case castor3d::PostEffect::Kind::eHDR:
				return cuT( "HDR" );
			case castor3d::PostEffect::Kind::eSRGB:
				return cuT( "sRGB" );
			case castor3d::PostEffect::Kind::eOverlay:
				return cuT( "Overlay" );
			default:
				CU_Failure( "Unexpected PostEffect::Kind." );
				return cuT( "Unknown" );
			}
		}
	}

	PostEffect::PostEffect( castor::String const & name
		, castor::String const & fullName
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & CU_UnusedParam( parameters )
		, uint32_t passesCount
		, Kind kind )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, castor::Named{ name }
		, m_fullName{ fullName }
		, m_renderTarget{ renderTarget }
		, m_passesCount{ passesCount }
		, m_kind{ kind }
		, m_enabled{ true }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	bool PostEffect::writeInto( castor::StringStream & file, castor::String const & tabs )
	{
		return doWriteInto( file, tabs );
	}

	bool PostEffect::initialise( castor3d::RenderDevice const & device
		, crg::ImageViewId const & texture
		, crg::FramePass const & previousPass )
	{
		m_target = &texture;
		auto name = m_fullName;
		name = castor::string::replace( name, cuT( "PostEffect" ), castor::String{} );
		name = castor::string::replace( name, cuT( "Post Effect" ), castor::String{} );
		m_timer = std::make_unique< RenderPassTimer >( device
			, getKindName( m_kind ) + cuT( " PostEffect" )
			, name
			, m_passesCount );
		m_result = doInitialise( device
			, *m_timer
			, previousPass );
		return m_result != nullptr;
	}

	void PostEffect::cleanup( castor3d::RenderDevice const & device )
	{
		doCleanup( device );
		m_timer.reset();
	}

	RenderPassTimerBlock PostEffect::start()
	{
		m_currentPass = 0u;
		return m_timer->start();
	}

	void PostEffect::update( CpuUpdater & updater )
	{
	}

	void PostEffect::update( GpuUpdater & updater )
	{
	}

	void PostEffect::doCopyResultToTarget( ashes::ImageView const & result
		, ashes::ImageView const & target
		, ashes::CommandBuffer & commandBuffer )
	{
		// Put result image in transfer source layout.
		commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, result.makeTransferSource( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		// Put target image in transfer destination layout.
		commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, target.makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Copy result to target.
		commandBuffer.copyImage( result, target );
		// Put target image in fragment shader input layout.
		commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, target.makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
	}
}
