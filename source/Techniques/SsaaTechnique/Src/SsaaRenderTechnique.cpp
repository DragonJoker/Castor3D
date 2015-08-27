#include "SsaaRenderTechnique.hpp"

#include <FrameBuffer.hpp>
#include <ColourRenderBuffer.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <RenderBufferAttachment.hpp>
#include <TextureAttachment.hpp>
#include <RenderTarget.hpp>
#include <DepthStencilState.hpp>
#include <RasteriserState.hpp>
#include <Parameter.hpp>

#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Ssaa
{
	RenderTechnique::RenderTechnique()
		:	RenderTechniqueBase( cuT( "ssaa" ) )
		,	m_iSamplesCount( 0 )
	{
	}

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		:	RenderTechniqueBase( cuT( "ssaa" ), p_renderTarget, p_pRenderSystem, p_params )
		,	m_iSamplesCount( 0 )
	{
		if ( p_params.Get( cuT( "samples_count" ), m_iSamplesCount ) && m_iSamplesCount > 1 )
		{
			Logger::LogInfo( "Using SSAA, %d samples", m_iSamplesCount );
			m_pSsFrameBuffer	= m_pRenderTarget->CreateFrameBuffer();
			m_pSsColorBuffer	= m_pSsFrameBuffer->CreateColourRenderBuffer( ePIXEL_FORMAT_A8R8G8B8 );
			m_pSsColorAttach	= m_pRenderTarget->CreateAttachment( m_pSsColorBuffer );
			m_pSsDepthBuffer	= m_pSsFrameBuffer->CreateDepthStencilRenderBuffer(	ePIXEL_FORMAT_DEPTH24 );
			m_pSsDepthAttach	= m_pRenderTarget->CreateAttachment( m_pSsDepthBuffer );
			m_pBoundFrameBuffer = m_pSsFrameBuffer;
		}
		else
		{
			Logger::LogWarning( "Using classic rendering through SSAA Render Technique" );
			m_iSamplesCount = 0;
			m_pBoundFrameBuffer = m_pFrameBuffer;
		}
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_bReturn = true;

		if ( m_iSamplesCount )
		{
			l_bReturn &= m_pSsFrameBuffer->Create( 0 );
			l_bReturn &= m_pSsColorBuffer->Create();
			l_bReturn &= m_pSsDepthBuffer->Create();
		}

		return l_bReturn;
	}

	void RenderTechnique::DoDestroy()
	{
		if ( m_iSamplesCount )
		{
			m_pSsColorBuffer->Destroy();
			m_pSsDepthBuffer->Destroy();
			m_pSsFrameBuffer->Destroy();
		}
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_uiIndex )
	{
		bool l_bReturn = true;
		m_sizeSsaa = m_size;
		m_rectSsaa = m_rect;

		if ( m_iSamplesCount )
		{
			m_sizeSsaa = Size( m_sizeSsaa.width() * m_iSamplesCount, m_sizeSsaa.height() * m_iSamplesCount );
			m_rectSsaa.set( 0, 0, m_sizeSsaa.width(), m_sizeSsaa.height() );
			l_bReturn = m_pSsColorBuffer->Initialise( m_sizeSsaa );

			if ( l_bReturn )
			{
				l_bReturn = m_pSsDepthBuffer->Initialise( m_sizeSsaa );
			}

			if ( l_bReturn )
			{
				l_bReturn = m_pSsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

				if ( l_bReturn )
				{
					l_bReturn = m_pSsColorAttach->Attach( eATTACHMENT_POINT_COLOUR0, m_pSsFrameBuffer );

					if ( l_bReturn )
					{
						l_bReturn = m_pSsDepthAttach->Attach( eATTACHMENT_POINT_DEPTH, m_pSsFrameBuffer );
					}

					m_pSsFrameBuffer->Unbind();
				}
			}
		}

		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		if ( m_iSamplesCount )
		{
			m_pSsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
			m_pSsColorAttach->Detach();
			m_pSsDepthAttach->Detach();
			m_pSsFrameBuffer->Unbind();
			m_pSsColorBuffer->Cleanup();
			m_pSsDepthBuffer->Cleanup();
		}
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_pBoundFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_pBoundFrameBuffer->Unbind();

		if ( m_iSamplesCount )
		{
			m_pSsFrameBuffer->BlitInto( m_pFrameBuffer, m_rect, eBUFFER_COMPONENT_COLOUR );
		}
	}
}
