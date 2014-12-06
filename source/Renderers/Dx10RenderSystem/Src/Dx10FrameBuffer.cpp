#include "Dx10FrameBuffer.hpp"
#include "Dx10ColourRenderBuffer.hpp"
#include "Dx10DepthStencilRenderBuffer.hpp"
#include "Dx10FrameBuffer.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10DynamicTexture.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
{
	DxFrameBuffer::DxFrameBuffer( DxRenderSystem * p_pRenderSystem )
		:	FrameBuffer( p_pRenderSystem->GetEngine()	)
		,	m_pRenderSystem( p_pRenderSystem	)
		,	m_pDevice( p_pRenderSystem->GetDevice()	)
		,	m_pOldDepthStencilView( NULL	)
	{
	}

	DxFrameBuffer::~DxFrameBuffer()
	{
	}

	bool DxFrameBuffer::Create( int CU_PARAM_UNUSED( p_iSamplesCount ) )
	{
		return true;
	}

	void DxFrameBuffer::Destroy()
	{
		SafeRelease( m_pOldDepthStencilView );
	}

	bool DxFrameBuffer::SetDrawBuffers( uint32_t CU_PARAM_UNUSED( p_uiSize ), eATTACHMENT_POINT const * CU_PARAM_UNUSED( p_eAttaches ) )
	{
		return SetDrawBuffers();
	}

	bool DxFrameBuffer::SetDrawBuffers()
	{
		bool l_bReturn = false;

		if ( m_mapRbo.size() || m_mapTex.size() )
		{
			ID3D10Device * l_pDevice = m_pRenderSystem->GetDevice();
			D3D10RenderTargetViewArray l_arraySurfaces;
			ID3D10DepthStencilView * l_pView = NULL;
			l_arraySurfaces.reserve( 10 );

			for ( TexAttachMapIt l_it = m_mapTex.begin(); l_it != m_mapTex.end(); ++l_it )
			{
				if ( l_it->first != eATTACHMENT_POINT_DEPTH && l_it->first != eATTACHMENT_POINT_STENCIL && l_it->first != eATTACHMENT_POINT_NONE )
				{
					l_arraySurfaces.push_back( std::static_pointer_cast< DxDynamicTexture >( l_it->second )->GetRenderTargetView() );
				}
			}

			for ( RboAttachMapIt l_it = m_mapRbo.begin(); l_it != m_mapRbo.end(); ++l_it )
			{
				if ( l_it->first == eATTACHMENT_POINT_DEPTH || l_it->first == eATTACHMENT_POINT_STENCIL )
				{
					l_pView = reinterpret_cast< ID3D10DepthStencilView * >( std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface() );
				}
				else if ( l_it->first != eATTACHMENT_POINT_NONE )
				{
					l_arraySurfaces.push_back( reinterpret_cast< ID3D10RenderTargetView * >( std::static_pointer_cast< DxColourRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface() ) );
				}
			}

			if ( l_arraySurfaces.size() )
			{
				m_arrayOldRenderTargets.resize( l_arraySurfaces.size() );
				l_pDevice->OMGetRenderTargets( UINT( l_arraySurfaces.size() ), m_arrayOldRenderTargets.data(), &m_pOldDepthStencilView );
				l_pDevice->OMSetRenderTargets( UINT( l_arraySurfaces.size() ), &l_arraySurfaces[0], l_pView );
				l_bReturn = true;
			}
			else if ( l_pView )
			{
				l_pDevice->OMGetRenderTargets( 0, NULL, &m_pOldDepthStencilView );
				l_pDevice->OMSetRenderTargets( 0, NULL, l_pView );
			}
		}

		return l_bReturn;
	}

	ColourRenderBufferSPtr DxFrameBuffer::CreateColourRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< DxColourRenderBuffer >( m_pRenderSystem, p_ePixelFormat );
	}

	DepthStencilRenderBufferSPtr DxFrameBuffer::CreateDepthStencilRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< DxDepthStencilRenderBuffer >( m_pRenderSystem, p_ePixelFormat );
	}

	ID3D10View * DxFrameBuffer::GetSurface( Castor3D::eATTACHMENT_POINT p_eAttach )
	{
		ID3D10View * l_pReturn = NULL;

		if ( m_mapRbo.size() || m_mapTex.size() && p_eAttach != eATTACHMENT_POINT_NONE )
		{
			if ( p_eAttach != eATTACHMENT_POINT_STENCIL && p_eAttach != eATTACHMENT_POINT_DEPTH )
			{
				for ( TexAttachMapIt l_it = m_mapTex.begin(); l_it != m_mapTex.end() && !l_pReturn; ++l_it )
				{
					if ( l_it->first == p_eAttach )
					{
						l_pReturn = std::static_pointer_cast< DxDynamicTexture >( l_it->second )->GetRenderTargetView();
					}
				}
			}

			for ( RboAttachMapIt l_it = m_mapRbo.begin(); l_it != m_mapRbo.end() && !l_pReturn; ++l_it )
			{
				if ( l_it->first == eATTACHMENT_POINT_DEPTH || l_it->first == eATTACHMENT_POINT_STENCIL )
				{
					l_pReturn = std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface();
				}
				else
				{
					l_pReturn = std::static_pointer_cast< DxColourRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface();
				}
			}
		}

		return l_pReturn;
	}

	bool DxFrameBuffer::DoBind( eFRAMEBUFFER_TARGET CU_PARAM_UNUSED( p_eTarget ) )
	{
		return true;
	}

	void DxFrameBuffer::DoUnbind()
	{
		ID3D10Device * l_pDevice = m_pRenderSystem->GetDevice();
		D3D10RenderTargetViewArray l_arraySurfaces;
		ID3D10DepthStencilView * l_pView = m_pOldDepthStencilView;

		for ( std::size_t i = 0; i < m_arrayOldRenderTargets.size(); ++i )
		{
			if ( m_arrayOldRenderTargets[i] )
			{
				l_arraySurfaces.push_back( m_arrayOldRenderTargets[i] );
			}
		}

		m_arrayOldRenderTargets.clear();

		if ( l_pView || !l_arraySurfaces.empty() )
		{
			l_pDevice->OMSetRenderTargets( UINT( l_arraySurfaces.size() ), l_arraySurfaces.empty() ? NULL : l_arraySurfaces.data(), l_pView );
		}

		for ( std::size_t i = 0; i < l_arraySurfaces.size(); ++i )
		{
			SafeRelease( l_arraySurfaces[i] );
		}

		SafeRelease( m_pOldDepthStencilView );
	}

	void DxFrameBuffer::DoAttachFba( Castor3D::FrameBufferAttachmentRPtr CU_PARAM_UNUSED( p_pAttach ) )
	{
	}

	void DxFrameBuffer::DoDetachFba( Castor3D::FrameBufferAttachmentRPtr CU_PARAM_UNUSED( p_pAttach ) )
	{
	}

	bool DxFrameBuffer::DoAttach( eATTACHMENT_POINT CU_PARAM_UNUSED( p_eAttachment ), DynamicTextureSPtr CU_PARAM_UNUSED( p_pTexture ), eTEXTURE_TARGET CU_PARAM_UNUSED( p_eTarget ), int CU_PARAM_UNUSED( p_iLayer ) )
	{
		return true;
	}

	bool DxFrameBuffer::DoAttach( eATTACHMENT_POINT CU_PARAM_UNUSED( p_eAttachment ), RenderBufferSPtr CU_PARAM_UNUSED( p_pRenderBuffer ) )
	{
		return true;
	}

	void DxFrameBuffer::DoDetachAll()
	{
	}

	bool DxFrameBuffer::DoStretchInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_eInterpolation ) )
	{
		SrcDstPairArray l_arrayPairs;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
		bool l_bDepth = ( p_uiComponents & eBUFFER_COMPONENT_DEPTH ) == eBUFFER_COMPONENT_DEPTH;
		bool l_bStencil = ( p_uiComponents & eBUFFER_COMPONENT_STENCIL ) == eBUFFER_COMPONENT_STENCIL;
		bool l_bColour = ( p_uiComponents & eBUFFER_COMPONENT_COLOUR ) == eBUFFER_COMPONENT_COLOUR;

		for ( AttachArrayConstIt l_it = m_arrayAttaches.begin(); l_it != m_arrayAttaches.end(); ++l_it )
		{
			eATTACHMENT_POINT l_eAttach = *l_it;

			if ( ( l_eAttach != eATTACHMENT_POINT_DEPTH && l_eAttach != eATTACHMENT_POINT_STENCIL ) )
			{
				if ( ( l_bDepth && l_eAttach == eATTACHMENT_POINT_DEPTH ) || ( l_bStencil && l_eAttach == eATTACHMENT_POINT_STENCIL ) || ( l_bColour && l_eAttach != eATTACHMENT_POINT_DEPTH && l_eAttach != eATTACHMENT_POINT_STENCIL ) )
				{
					ID3D10View * l_pSrc = GetSurface( *l_it );
					ID3D10View * l_pDst = l_pBuffer->GetSurface( *l_it );

					if ( l_pSrc && l_pDst )
					{
						l_arrayPairs.push_back( std::make_pair( l_pSrc, l_pDst ) );
					}
				}
			}
		}

		HRESULT l_hr = S_OK;

		for ( SrcDstPairArrayIt l_itArray = l_arrayPairs.begin(); l_itArray != l_arrayPairs.end() && l_hr == S_OK; ++l_itArray )
		{
			try
			{
				ID3D10Resource * l_pDstSurface;
				ID3D10Resource * l_pSrcSurface;
				l_itArray->first->GetResource( &l_pDstSurface );
				l_itArray->second->GetResource( &l_pSrcSurface );

				if ( l_pDstSurface && l_pSrcSurface )
				{
					D3D10_BOX l_box = { 0 };
					l_box.front = 0;
					l_box.back = 1;
					l_box.left = p_rectSrc.left();
					l_box.right = p_rectSrc.right();
					l_box.top = p_rectSrc.top();
					l_box.bottom = p_rectSrc.bottom();
					m_pRenderSystem->GetDevice()->CopySubresourceRegion( l_pDstSurface, 0, p_rectDst.left(), p_rectDst.top(), 0, l_pSrcSurface, 0, NULL );//&l_box );
				}

				SafeRelease( l_pSrcSurface );
				SafeRelease( l_pDstSurface );
			}
			catch ( ... )
			{
				Logger::LogError( cuT( "Error while stretching src to dst frame buffer" ) );
				l_hr = E_FAIL;
			}
		}

		return l_hr == S_OK;
	}
}
