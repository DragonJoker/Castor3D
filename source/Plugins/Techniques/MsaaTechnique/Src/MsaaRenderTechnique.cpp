#include "MsaaRenderTechnique.hpp"

#include <CameraManager.hpp>
#include <DepthStencilStateManager.hpp>
#include <LightManager.hpp>
#include <MaterialManager.hpp>
#include <OverlayManager.hpp>
#include <RasteriserStateManager.hpp>
#include <SceneManager.hpp>
#include <TargetManager.hpp>

#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/Viewport.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/PointFrameVariable.hpp>
#include <Texture/DynamicTexture.hpp>

#include <FontManager.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Msaa
{
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "msaa" ), p_renderTarget, p_renderSystem, p_params )
		, m_samplesCount( 0 )
	{
		String l_count;

		if ( p_params.Get( cuT( "samples_count" ), l_count ) )
		{
			m_samplesCount = string::to_int( l_count );
		}

		m_msFrameBuffer = m_renderSystem->CreateFrameBuffer();
		m_pMsColorBuffer = m_msFrameBuffer->CreateColourRenderBuffer( ePIXEL_FORMAT_ARGB16F32F );
		m_pMsDepthBuffer = m_msFrameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH32F );
		m_pMsColorAttach = m_msFrameBuffer->CreateAttachment( m_pMsColorBuffer );
		m_pMsDepthAttach = m_msFrameBuffer->CreateAttachment( m_pMsDepthBuffer );

		if ( m_samplesCount > 1 )
		{
			m_wpFrontRasteriserState.lock()->SetMultisample( true );
			m_wpBackRasteriserState.lock()->SetMultisample( true );
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

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
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

	void RenderTechnique::DoRender( stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		m_renderTarget->GetDepthStencilState()->Apply();
		Castor3D::RenderTechnique::DoRender( m_size, p_nodes, p_camera, p_frameTime );
	}

	void RenderTechnique::DoEndRender( Scene & p_scene )
	{
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->BlitInto( m_frameBuffer.m_frameBuffer, m_rect, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH );
	}
}
