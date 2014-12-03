#include "Dx9FrameBuffer.hpp"
#include "Dx9ColourRenderBuffer.hpp"
#include "Dx9DepthStencilRenderBuffer.hpp"
#include "Dx9RenderSystem.hpp"
#include "Dx9DynamicTexture.hpp"

#include <Logger.hpp>
#include <RenderBufferAttachment.hpp>
#include <TextureAttachment.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	DxFrameBuffer::DxFrameBuffer( DxRenderSystem * p_pRenderSystem )
		:	FrameBuffer( p_pRenderSystem->GetEngine() )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pDevice( p_pRenderSystem->GetDevice() )
		,	m_bModified( false )
		,	m_pOldDepthStencilSurface( NULL )
	{
		for ( std::size_t i = 0; i < m_arrayOldSurfaces.size(); ++i )
		{
			m_arrayOldSurfaces[i] = NULL;
		}
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
			IDirect3DDevice9 * l_pContext = m_pRenderSystem->GetDevice();
			D3DSurfaceArray l_arraySurfaces = { 0 };
			IDirect3DSurface9 * l_pView = NULL;

			for ( TexAttachMapIt l_it = m_mapTex.begin(); l_it != m_mapTex.end(); ++l_it )
			{
				if ( l_it->first != eATTACHMENT_POINT_DEPTH && l_it->first != eATTACHMENT_POINT_STENCIL && l_it->first != eATTACHMENT_POINT_NONE )
				{
					l_arraySurfaces[DirectX9::Get( l_it->first )] = std::static_pointer_cast< DxDynamicTexture >( l_it->second )->GetDxSurface();
				}
			}

			for ( RboAttachMapIt l_it = m_mapRbo.begin(); l_it != m_mapRbo.end(); ++l_it )
			{
				if ( l_it->first == eATTACHMENT_POINT_DEPTH || l_it->first == eATTACHMENT_POINT_STENCIL )
				{
					l_pView = std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface();
				}
				else if ( l_it->first != eATTACHMENT_POINT_NONE )
				{
					l_arraySurfaces[DirectX9::Get( l_it->first )] = std::static_pointer_cast< DxColourRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface();
				}
			}

			if ( l_pView )
			{
				l_pContext->GetDepthStencilSurface( &m_pOldDepthStencilSurface );
				l_pContext->SetDepthStencilSurface( l_pView );
			}

			for ( DWORD i = 0; i < l_arraySurfaces.size(); ++i )
			{
				if ( l_arraySurfaces[i] )
				{
					l_pContext->GetRenderTarget( i, &m_arrayOldSurfaces[i] );
					l_pContext->SetRenderTarget( i, l_arraySurfaces[i] );
				}
				else
				{
					m_arrayOldSurfaces[i] = NULL;
				}
			}
		}

		return l_bReturn;
	}

	IDirect3DSurface9 * DxFrameBuffer::GetSurface( eATTACHMENT_POINT p_eAttachmentPoint )
	{
		IDirect3DSurface9 * l_pReturn = NULL;
		TexAttachMapIt l_itTex = m_mapTex.find( p_eAttachmentPoint );

		if ( l_itTex != m_mapTex.end() )
		{
			l_pReturn = std::static_pointer_cast< DxDynamicTexture >( l_itTex->second )->GetDxSurface();
		}
		else
		{
			RboAttachMapIt l_itBuf = m_mapRbo.find( p_eAttachmentPoint );

			if ( l_itBuf != m_mapRbo.end() )
			{
				if ( p_eAttachmentPoint == eATTACHMENT_POINT_DEPTH || p_eAttachmentPoint == eATTACHMENT_POINT_STENCIL )
				{
					l_pReturn = std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_itBuf->second )->GetDxRenderBuffer().GetSurface();
				}
				else
				{
					l_pReturn = std::static_pointer_cast< DxColourRenderBuffer >( l_itBuf->second )->GetDxRenderBuffer().GetSurface();
				}
			}
		}

		return l_pReturn;
	}

	ColourRenderBufferSPtr DxFrameBuffer::CreateColourRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< DxColourRenderBuffer >( m_pRenderSystem, p_ePixelFormat );
	}

	DepthStencilRenderBufferSPtr DxFrameBuffer::CreateDepthStencilRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< DxDepthStencilRenderBuffer >( m_pRenderSystem, p_ePixelFormat );
	}

	bool DxFrameBuffer::DoBind( eFRAMEBUFFER_TARGET CU_PARAM_UNUSED( p_eBindingMode ) )
	{
		return true;
	}

	void DxFrameBuffer::DoUnbind()
	{
		IDirect3DDevice9 * l_pContext = m_pRenderSystem->GetDevice();

		if ( m_pOldDepthStencilSurface )
		{
			l_pContext->SetDepthStencilSurface( m_pOldDepthStencilSurface );
			SafeRelease( m_pOldDepthStencilSurface );
		}

		for ( DWORD i = 0; i < m_arrayOldSurfaces.size(); ++i )
		{
			if ( m_arrayOldSurfaces[i] )
			{
				l_pContext->SetRenderTarget( i, m_arrayOldSurfaces[i] );
				SafeRelease( m_arrayOldSurfaces[i] );
			}
		}
	}

	void DxFrameBuffer::DoAttach( RenderBufferAttachmentRPtr p_pAttach )
	{
		m_mapAttRbo[DirectX9::Get( p_pAttach->GetAttachmentPoint() )] = p_pAttach->GetRenderBuffer();
	}

	void DxFrameBuffer::DoDetach( RenderBufferAttachmentRPtr p_pAttach )
	{
		RenderBufferIdxMapIt l_it = m_mapAttRbo.find( DirectX9::Get( p_pAttach->GetAttachmentPoint() ) );

		if ( l_it != m_mapAttRbo.end() )
		{
			m_mapAttRbo.erase( l_it );
		}
	}

	void DxFrameBuffer::DoAttach( TextureAttachmentRPtr p_pAttach )
	{
		m_mapAttTex[DirectX9::Get( p_pAttach->GetAttachmentPoint() )] = p_pAttach->GetTexture();
	}

	void DxFrameBuffer::DoDetach( TextureAttachmentRPtr p_pAttach )
	{
		TextureIdxMapIt l_it = m_mapAttTex.find( DirectX9::Get( p_pAttach->GetAttachmentPoint() ) );

		if ( l_it != m_mapAttTex.end() )
		{
			m_mapAttTex.erase( l_it );
		}
	}

	bool DxFrameBuffer::DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::DynamicTextureSPtr p_pTexture, Castor3D::eTEXTURE_TARGET p_eTarget, int p_iLayer )
	{
		m_mapAttTex[DirectX9::Get( p_eAttachment )] = p_pTexture;
		return true;
	}

	bool DxFrameBuffer::DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::RenderBufferSPtr p_pRenderBuffer )
	{
		m_mapAttRbo[DirectX9::Get( p_eAttachment )] = p_pRenderBuffer;
		return true;
	}

	void DxFrameBuffer::DoDetachAll()
	{
		m_mapAttTex.clear();
		m_mapAttRbo.clear();
	}

	bool DxFrameBuffer::DoStretchInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE p_eInterpolation )
	{
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
		BlitSchemeMapIt l_it = m_mapBlitSchemes.find( l_pBuffer );
		HRESULT l_hr = S_OK;
		RECT l_rcSrc = { p_rectSrc.left(), p_rectSrc.top(), p_rectSrc.right(), p_rectSrc.bottom() };
		RECT l_rcDst = { p_rectDst.left(), p_rectDst.top(), p_rectDst.right(), p_rectDst.bottom() };

		if ( l_it == m_mapBlitSchemes.end() || m_bModified || l_pBuffer->m_bModified )
		{
			DECLARE_SET( eATTACHMENT_POINT, Attachment );
			AttachmentSet l_setBlit;

			if ( m_bModified || l_pBuffer->m_bModified )
			{
				m_bModified = false;
				l_pBuffer->m_bModified = false;

				if ( l_it != m_mapBlitSchemes.end() )
				{
					m_mapBlitSchemes.erase( l_it );
				}
			}

			m_mapBlitSchemes.insert( std::make_pair( l_pBuffer, SrcDstPairArray() ) );
			l_it = m_mapBlitSchemes.find( l_pBuffer );
			std::for_each( m_mapTex.begin(), m_mapTex.end(), [&]( std::pair< eATTACHMENT_POINT, DynamicTextureSPtr > p_pairAttachment )
			{
				if ( ( ( p_uiComponents & eBUFFER_COMPONENT_COLOUR ) && p_pairAttachment.first >= eATTACHMENT_POINT_COLOUR0 && p_pairAttachment.first <= eATTACHMENT_POINT_COLOUR15 )
						// 				|| ((p_uiComponents & eBUFFER_COMPONENT_DEPTH) && p_pairAttachment.first == eATTACHMENT_POINT_DEPTH)
						// 				|| ((p_uiComponents & eBUFFER_COMPONENT_STENCIL) && p_pairAttachment.first == eATTACHMENT_POINT_STENCIL)
				   )
				{
					AttachmentSetIt l_itSet = l_setBlit.find( p_pairAttachment.first );
					IDirect3DSurface9 * l_pSurface = std::static_pointer_cast< DxDynamicTexture >( p_pairAttachment.second )->GetDxSurface();

					if ( l_itSet == l_setBlit.end() )
					{
						TexAttachMapIt l_itTex = l_pBuffer->m_mapTex.find( p_pairAttachment.first );

						if ( l_itTex != l_pBuffer->m_mapTex.end() )
						{
							l_it->second.push_back( std::make_pair( l_pSurface, std::static_pointer_cast< DxDynamicTexture >( l_itTex->second )->GetDxSurface() ) );
							l_setBlit.insert( p_pairAttachment.first );
							l_itSet = l_setBlit.find( p_pairAttachment.first );
						}
					}

					if ( l_itSet == l_setBlit.end() )
					{
						RboAttachMapIt l_itBuf = l_pBuffer->m_mapRbo.find( p_pairAttachment.first );

						if ( l_itBuf != l_pBuffer->m_mapRbo.end() )
						{
							if ( p_pairAttachment.first == eATTACHMENT_POINT_DEPTH || p_pairAttachment.first == eATTACHMENT_POINT_STENCIL )
							{
								l_it->second.push_back( std::make_pair( l_pSurface, std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_itBuf->second )->GetDxRenderBuffer().GetSurface() ) );
							}
							else
							{
								l_it->second.push_back( std::make_pair( l_pSurface, std::static_pointer_cast< DxColourRenderBuffer >( l_itBuf->second )->GetDxRenderBuffer().GetSurface() ) );
							}

							l_setBlit.insert( p_pairAttachment.first );
						}
					}
				}
			} );
			std::for_each( m_mapRbo.begin(), m_mapRbo.end(), [&]( std::pair< eATTACHMENT_POINT, RenderBufferSPtr > p_pairAttachment )
			{
				if ( ( ( p_uiComponents & eBUFFER_COMPONENT_COLOUR ) && p_pairAttachment.first >= eATTACHMENT_POINT_COLOUR0 && p_pairAttachment.first <= eATTACHMENT_POINT_COLOUR15 )
						// 				|| ((p_uiComponents & eBUFFER_COMPONENT_DEPTH) && p_pairAttachment.first == eATTACHMENT_POINT_DEPTH)
						// 				|| ((p_uiComponents & eBUFFER_COMPONENT_STENCIL) && p_pairAttachment.first == eATTACHMENT_POINT_STENCIL)
				   )
				{
					AttachmentSetIt l_itSet = l_setBlit.find( p_pairAttachment.first );
					IDirect3DSurface9 * l_pSurface = NULL;

					if ( p_pairAttachment.first == eATTACHMENT_POINT_DEPTH || p_pairAttachment.first == eATTACHMENT_POINT_STENCIL )
					{
						l_pSurface = std::static_pointer_cast< DxDepthStencilRenderBuffer >( p_pairAttachment.second )->GetDxRenderBuffer().GetSurface();
					}
					else
					{
						l_pSurface = std::static_pointer_cast< DxColourRenderBuffer >( p_pairAttachment.second )->GetDxRenderBuffer().GetSurface();
					}

					if ( l_itSet == l_setBlit.end() )
					{
						TexAttachMapIt l_itTex = l_pBuffer->m_mapTex.find( p_pairAttachment.first );

						if ( l_itTex != l_pBuffer->m_mapTex.end() )
						{
							l_it->second.push_back( std::make_pair( l_pSurface, std::static_pointer_cast< DxDynamicTexture >( l_itTex->second )->GetDxSurface() ) );
							l_setBlit.insert( p_pairAttachment.first );
							l_itSet = l_setBlit.find( p_pairAttachment.first );
						}
					}

					if ( l_itSet == l_setBlit.end() )
					{
						RboAttachMapIt l_itBuf = l_pBuffer->m_mapRbo.find( p_pairAttachment.first );

						if ( l_itBuf != l_pBuffer->m_mapRbo.end() )
						{
							if ( p_pairAttachment.first == eATTACHMENT_POINT_DEPTH || p_pairAttachment.first == eATTACHMENT_POINT_STENCIL )
							{
								l_it->second.push_back( std::make_pair( l_pSurface, std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_itBuf->second )->GetDxRenderBuffer().GetSurface() ) );
							}
							else
							{
								l_it->second.push_back( std::make_pair( l_pSurface, std::static_pointer_cast< DxColourRenderBuffer >( l_itBuf->second )->GetDxRenderBuffer().GetSurface() ) );
							}

							l_setBlit.insert( p_pairAttachment.first );
						}
					}
				}
			} );
		}

		if ( l_it != m_mapBlitSchemes.end() )
		{
			for ( SrcDstPairArrayIt l_itArray = l_it->second.begin(); l_itArray != l_it->second.end() && l_hr == S_OK; ++l_itArray )
			{
				try
				{
					l_hr = m_pDevice->StretchRect( l_itArray->first, &l_rcSrc, l_itArray->second, &l_rcDst, D3DTEXTUREFILTERTYPE( DirectX9::Get( p_eInterpolation ) ) );
					dxCheckError( l_hr, "Error while stretching src to dst frame buffer" );
				}
				catch ( ... )
				{
					Logger::LogError( cuT( "Error while stretching src to dst frame buffer" ) );
				}
			}
		}

		return l_hr == S_OK;
	}
}
