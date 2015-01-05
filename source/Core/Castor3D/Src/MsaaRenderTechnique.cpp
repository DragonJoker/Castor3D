#include "MsaaRenderTechnique.hpp"
#include "FrameBuffer.hpp"
#include "RasteriserState.hpp"
#include "RenderTarget.hpp"
#include "ColourRenderBuffer.hpp"
#include "DepthStencilRenderBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "Engine.hpp"
#include "Parameter.hpp"
#include "DynamicTexture.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	MsaaRenderTechnique::MsaaRenderTechnique()
		:	RenderTechniqueBase( cuT( "msaa" ) )
		,	m_iSamplesCount( 0 )
	{
	}

	MsaaRenderTechnique::MsaaRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		:	RenderTechniqueBase( cuT( "msaa" ), p_renderTarget, p_pRenderSystem, p_params )
		,	m_iSamplesCount( 0 )
	{
		if ( p_params.Get( cuT( "samples_count" ), m_iSamplesCount ) && m_iSamplesCount > 1 )
		{
			Logger::LogMessage( "Using MSAA, %d samples", m_iSamplesCount );
			m_pMsFrameBuffer	= m_pRenderTarget->CreateFrameBuffer();
			m_pMsColorBuffer	= m_pFrameBuffer->CreateColourRenderBuffer(	ePIXEL_FORMAT_A8R8G8B8 );
			m_pMsDepthBuffer	= m_pFrameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH24 );
			m_pMsColorAttach	= m_pRenderTarget->CreateAttachment( m_pMsColorBuffer );
			m_pMsDepthAttach	= m_pRenderTarget->CreateAttachment( m_pMsDepthBuffer );
			RasteriserStateSPtr l_pRasteriser = m_pEngine->CreateRasteriserState( cuT( "MSAA_RT" ) );
			l_pRasteriser->SetMultisample( true );
			m_wpMsRasteriserState = l_pRasteriser;
			m_pBoundFrameBuffer = m_pMsFrameBuffer;
		}
		else
		{
			Logger::LogWarning( "Using classic rendering through MSAA Render Technique" );
			m_iSamplesCount = 0;
			m_pBoundFrameBuffer = m_pFrameBuffer;
		}
	}

	MsaaRenderTechnique::~MsaaRenderTechnique()
	{
	}

	RenderTechniqueBaseSPtr MsaaRenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new MsaaRenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
	}

	bool MsaaRenderTechnique::DoCreate()
	{
		bool l_bReturn = true;

		if ( m_iSamplesCount )
		{
			l_bReturn &= m_pMsFrameBuffer->Create( m_iSamplesCount );
			m_pMsColorBuffer->SetSamplesCount( m_iSamplesCount );
			m_pMsDepthBuffer->SetSamplesCount( m_iSamplesCount );
			l_bReturn &= m_pMsColorBuffer->Create();
			l_bReturn &= m_pMsDepthBuffer->Create();
		}

		return l_bReturn;
	}

	void MsaaRenderTechnique::DoDestroy()
	{
		if ( m_iSamplesCount )
		{
			m_pMsColorBuffer->Destroy();
			m_pMsDepthBuffer->Destroy();
			m_pMsFrameBuffer->Destroy();
		}
	}

	bool MsaaRenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_bReturn = true;

		if ( m_iSamplesCount )
		{
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
				l_bReturn = m_pMsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

				if ( l_bReturn )
				{
					l_bReturn = m_pMsColorAttach->Attach( eATTACHMENT_POINT_COLOUR0,	m_pMsFrameBuffer );

					if ( l_bReturn )
					{
						l_bReturn = m_pMsDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,		m_pMsFrameBuffer );
					}

					m_pMsFrameBuffer->Unbind();
				}
			}
		}

		return l_bReturn;
	}

	void MsaaRenderTechnique::DoCleanup()
	{
		if ( m_iSamplesCount )
		{
			m_pMsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
			m_pMsColorAttach->Detach();
			m_pMsDepthAttach->Detach();
			m_pMsFrameBuffer->Unbind();
			m_pMsFrameBuffer->DetachAll();
			m_pMsColorBuffer->Cleanup();
			m_pMsDepthBuffer->Cleanup();
		}
	}

	bool MsaaRenderTechnique::DoBeginRender()
	{
#if DX_DEBUG
		bool l_bReturn = true;

		if ( m_iSamplesCount )
		{
			return m_pMsFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
		}

		return l_bReturn;
#else
		return m_pBoundFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
#endif
	}

	bool MsaaRenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
#if DX_DEBUG

		if ( m_iSamplesCount )
		{
			m_pRenderTarget->GetDepthStencilState()->Apply();
			m_wpMsRasteriserState.lock()->Apply();
		}

#else
		m_pRenderTarget->GetDepthStencilState()->Apply();

		if ( !m_iSamplesCount )
		{
			m_pRenderTarget->GetRasteriserState()->Apply();
		}
		else
		{
			m_wpMsRasteriserState.lock()->Apply();
		}

#endif
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void MsaaRenderTechnique::DoEndRender()
	{
#if DX_DEBUG

		if ( m_iSamplesCount )
		{
			m_pMsFrameBuffer->Unbind();
			m_pMsFrameBuffer->BlitInto( m_pFrameBuffer, m_rect, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH );
			m_pRenderSystem->GetCurrentContext()->BToBRender( m_size, m_pColorBuffer, eBUFFER_COMPONENT_COLOUR );
		}

#else
		m_pBoundFrameBuffer->Unbind();

		if ( m_iSamplesCount )
		{
			m_pMsFrameBuffer->BlitInto( m_pFrameBuffer, m_rect, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH );
#	if DEBUG_BUFFERS
			m_pColorAttach->DownloadBuffer( m_pColorBuffer->GetBuffer() );
			const Image l_image( cuT( "Tmp" ), *m_pColorBuffer->GetBuffer() );
			Image::BinaryLoader()( l_image, File::GetUserDirectory() / cuT( "MsaaImage.bmp" ) );
#	endif
		}

#endif
	}
}
