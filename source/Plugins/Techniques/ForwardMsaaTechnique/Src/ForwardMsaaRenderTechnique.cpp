#include "ForwardMsaaRenderTechnique.hpp"

#include <Cache/CameraCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/OverlayCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/TargetCache.hpp>

#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Scene.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Technique/ForwardRenderTechniquePass.hpp>
#include <Texture/TextureLayout.hpp>

#include <Graphics/FontCache.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace forward_msaa
{
	int & GetSamplesCountParam( Parameters const & p_params, int & p_count )
	{
		String l_count;

		if ( p_params.Get( cuT( "samples_count" ), l_count ) )
		{
			p_count = string::to_int( l_count );
		}

		if ( p_count <= 1 )
		{
			p_count = 0;
		}

		return p_count;
	}

	String const RenderTechnique::Type = cuT( "forward_msaa" );
	String const RenderTechnique::Name = cuT( "MSAA Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( RenderTechnique::Type
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "forward_msaa_opaque" ), p_renderTarget, *this, true, GetSamplesCountParam( p_params, m_samplesCount ) > 1 )
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "forward_msaa_transparent" ), p_renderTarget, *this, false, GetSamplesCountParam( p_params, m_samplesCount ) > 1 )
			, p_params
			, GetSamplesCountParam( p_params, m_samplesCount ) > 1 )
	{
		m_msFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_pMsColorBuffer = m_msFrameBuffer->CreateColourRenderBuffer( PixelFormat::eRGBA16F32F );
		m_pMsDepthBuffer = m_msFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_pMsColorAttach = m_msFrameBuffer->CreateAttachment( m_pMsColorBuffer );
		m_pMsDepthAttach = m_msFrameBuffer->CreateAttachment( m_pMsDepthBuffer );

		Logger::LogInfo( StringStream() << cuT( "Using MSAA, " ) << m_samplesCount << cuT( " samples" ) );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_bReturn = m_msFrameBuffer->Create();
		m_pMsColorBuffer->SetSamplesCount( m_samplesCount );
		m_pMsDepthBuffer->SetSamplesCount( m_samplesCount );
		l_bReturn &= m_pMsColorBuffer->Create();
		l_bReturn &= m_pMsDepthBuffer->Create();
		return l_bReturn;
	}

	void RenderTechnique::DoDestroy()
	{
		m_pMsColorBuffer->Destroy();
		m_pMsDepthBuffer->Destroy();
		m_msFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_bReturn = true;
		m_rect = Castor::Rectangle( Position(), m_size );

		if ( l_bReturn )
		{
			l_bReturn = m_pMsColorBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_pMsDepthBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_msFrameBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			m_msFrameBuffer->Bind( FrameBufferMode::eConfig );
			m_msFrameBuffer->Attach( AttachmentPoint::eColour, m_pMsColorAttach );
			m_msFrameBuffer->Attach( AttachmentPoint::eDepth, m_pMsDepthAttach );
			m_msFrameBuffer->Unbind();
		}

		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		m_msFrameBuffer->Bind( FrameBufferMode::eConfig );
		m_msFrameBuffer->DetachAll();
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->Cleanup();
		m_pMsColorBuffer->Cleanup();
		m_pMsDepthBuffer->Cleanup();
	}

	void RenderTechnique::DoRenderOpaque( uint32_t & p_visible )
	{
		m_opaquePass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_msFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_msFrameBuffer->SetClearColour( m_renderTarget.GetScene()->GetBackgroundColour() );
		m_msFrameBuffer->Clear();
		m_opaquePass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
	}

	void RenderTechnique::DoRenderTransparent( uint32_t & p_visible )
	{
		m_msFrameBuffer->Unbind();
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_msFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer, m_rect, BufferComponent::eColour | BufferComponent::eDepth );
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -samples_count " ) + string::to_string( m_samplesCount ) ) > 0;
	}
}
