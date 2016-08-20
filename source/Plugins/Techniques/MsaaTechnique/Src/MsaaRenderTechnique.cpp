#include "MsaaRenderTechnique.hpp"

#include <CameraCache.hpp>
#include <LightCache.hpp>
#include <MaterialCache.hpp>
#include <OverlayCache.hpp>
#include <SceneCache.hpp>
#include <TargetCache.hpp>

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
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/PointFrameVariable.hpp>
#include <State/DepthStencilState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/TextureLayout.hpp>

#include <Graphics/FontCache.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Msaa
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

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "msaa" ), p_renderTarget, p_renderSystem, p_params, GetSamplesCountParam( p_params, m_samplesCount ) > 1 )
	{
		m_msFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_pMsColorBuffer = m_msFrameBuffer->CreateColourRenderBuffer( PixelFormat::ARGB16F32F );
		m_pMsDepthBuffer = m_msFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::D32F );
		m_pMsColorAttach = m_msFrameBuffer->CreateAttachment( m_pMsColorBuffer );
		m_pMsDepthAttach = m_msFrameBuffer->CreateAttachment( m_pMsDepthBuffer );

		if ( m_samplesCount > 1 )
		{
			m_multisampleState->SetMultisample( true );
		}
		else
		{
			m_samplesCount = 0;
		}

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
			l_bReturn = m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

			if ( l_bReturn )
			{
				l_bReturn = m_msFrameBuffer->Attach( eATTACHMENT_POINT_COLOUR, m_pMsColorAttach );

				if ( l_bReturn )
				{
					l_bReturn = m_msFrameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_pMsDepthAttach );
				}

				m_msFrameBuffer->Unbind();
			}
		}

		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_msFrameBuffer->DetachAll();
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->Cleanup();
		m_pMsColorBuffer->Cleanup();
		m_pMsDepthBuffer->Cleanup();
	}

	bool RenderTechnique::DoBeginRender( Scene & p_scene )
	{
		bool l_return = m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			m_msFrameBuffer->SetClearColour( p_scene.GetBackgroundColour() );
			m_msFrameBuffer->Clear();
		}

		return l_return;
	}

	void RenderTechnique::DoRender( SceneRenderNodes & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		m_renderTarget.GetDepthStencilState()->Apply();
		Castor3D::RenderTechnique::DoRender( m_size, p_nodes, p_camera, p_frameTime );
	}

	void RenderTechnique::DoEndRender( Scene & p_scene )
	{
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->BlitInto( m_frameBuffer.m_frameBuffer, m_rect, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH );
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -samples_count " ) + string::to_string( m_samplesCount ) ) > 0;
	}
}
