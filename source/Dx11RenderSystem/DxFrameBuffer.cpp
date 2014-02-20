#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxFrameBuffer.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"
#include "Dx11RenderSystem/DxTextureRenderer.hpp"

using namespace Dx11Render;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

DxRenderBuffer :: DxRenderBuffer( DxRenderSystem * p_pRenderSystem, DXGI_FORMAT p_eFormat, eBUFFER_COMPONENT p_eComponent, RenderBuffer & p_renderBuffer )
	:	m_size			( 0, 0				)
	,	m_pSurface		( NULL				)
	,	m_pOldSurface	( NULL				)
	,	m_pRenderSystem	( p_pRenderSystem	)
	,	m_eFormat		( p_eFormat			)
	,	m_eComponent	( p_eComponent		)
	,	m_renderBuffer	( p_renderBuffer	)
{
}

DxRenderBuffer :: ~DxRenderBuffer()
{
}

bool DxRenderBuffer :: Create()
{
	return true;
}

void DxRenderBuffer :: Destroy()
{
}

bool DxRenderBuffer :: Initialise( Size const & p_size )
{
	bool l_bReturn = m_size == p_size && m_pSurface != NULL;

	if( !l_bReturn )
	{
		m_size = p_size;

		if( m_pSurface )
		{
			Cleanup();
		}

		if( !m_pSurface )
		{
			DWORD l_dwMsType	= 1;
			DWORD l_dwMsQuality	= 0;

			if( m_renderBuffer.GetSamplesCount() > 1 )
			{
				l_dwMsType = m_renderBuffer.GetSamplesCount();
				
				if( m_pRenderSystem->GetDevice()->CheckMultisampleQualityLevels( m_eFormat, l_dwMsType, (UINT*)&l_dwMsQuality ) == S_OK )
				{
					if( l_dwMsQuality > 0 )
					{
						l_dwMsQuality--;
					}
				}
				else
				{
					l_dwMsType		= 1;
					l_dwMsQuality	= 0;
				}
			}
			
			UINT uiWidth = p_size.width();
			UINT uiHeight = p_size.height();
			HRESULT l_hr;

			if( m_eComponent == eBUFFER_COMPONENT_COLOUR )
			{
				D3D11_TEXTURE2D_DESC l_descTex;
				l_descTex.Width					= uiWidth;
				l_descTex.Height				= uiHeight;
				l_descTex.MipLevels				= 1;
				l_descTex.ArraySize				= 1;
				l_descTex.Format				= m_eFormat;
				l_descTex.SampleDesc.Count		= l_dwMsType;
				l_descTex.SampleDesc.Quality	= l_dwMsQuality;
				l_descTex.Usage					= D3D11_USAGE_DEFAULT;
				l_descTex.BindFlags				= D3D11_BIND_RENDER_TARGET;
				l_descTex.CPUAccessFlags		= 0;
				l_descTex.MiscFlags				= 0;

				l_hr = m_pRenderSystem->GetDevice()->CreateTexture2D( &l_descTex, NULL, &m_pTexture );
				dxDebugName( m_pTexture, RTTexture );

				if( l_hr == S_OK )
				{
					l_hr = m_pRenderSystem->GetDevice()->CreateRenderTargetView( m_pTexture, NULL, reinterpret_cast< ID3D11RenderTargetView** >( &m_pSurface ) );
					dxDebugName( m_pSurface, RTView );
				}
			}
			else
			{
				D3D11_TEXTURE2D_DESC l_descDepth;
				l_descDepth.Width				= uiWidth;
				l_descDepth.Height				= uiHeight;
				l_descDepth.MipLevels			= 1;
				l_descDepth.ArraySize			= 1;
				l_descDepth.Format				= m_eFormat;
				l_descDepth.SampleDesc.Count	= 1;
				l_descDepth.SampleDesc.Quality	= 0;
				l_descDepth.Usage				= D3D11_USAGE_DEFAULT;
				l_descDepth.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
				l_descDepth.CPUAccessFlags		= 0;
				l_descDepth.MiscFlags			= 0;

				l_hr = m_pRenderSystem->GetDevice()->CreateTexture2D( &l_descDepth, NULL, &m_pTexture );
				dxDebugName( m_pTexture, DSTexture );

				if( l_hr == S_OK )
				{
					l_hr = m_pRenderSystem->GetDevice()->CreateDepthStencilView( m_pTexture, NULL, reinterpret_cast< ID3D11DepthStencilView** >( &m_pSurface ) );
					dxDebugName( m_pSurface, DSView );
				}
			}

			l_bReturn = l_hr == S_OK;
		}
	}

	return l_bReturn;
}

void DxRenderBuffer :: Cleanup()
{
	SafeRelease( m_pSurface );
	SafeRelease( m_pTexture );
	SafeRelease( m_pOldSurface );
}

bool DxRenderBuffer :: Bind()
{
	return true;
	HRESULT l_hr = S_FALSE;

	if( m_pSurface )
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );

		if( m_eComponent == eBUFFER_COMPONENT_COLOUR )
		{
			ID3D11DepthStencilView * l_pView;
			l_pDeviceContext->OMGetRenderTargets( 1, reinterpret_cast< ID3D11RenderTargetView** >( &m_pOldSurface ), &l_pView );
			l_pDeviceContext->OMSetRenderTargets( 1, reinterpret_cast< ID3D11RenderTargetView** >( &m_pSurface ), l_pView );
			SafeRelease( l_pView );
		}
		else
		{
			ID3D11RenderTargetView * l_pView;
			l_pDeviceContext->OMGetRenderTargets( 1, &l_pView, reinterpret_cast< ID3D11DepthStencilView** >( &m_pSurface ) );
			l_pDeviceContext->OMSetRenderTargets( 1, &l_pView, reinterpret_cast< ID3D11DepthStencilView* >( m_pOldSurface ) );
			SafeRelease( l_pView );
		}

		l_pDeviceContext->Release();
		return true;
	}

	return l_hr == S_OK;
}

void DxRenderBuffer :: Unbind()
{
	return;
	if( m_pOldSurface )
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );

		if( m_eComponent == eBUFFER_COMPONENT_COLOUR )
		{
			ID3D11DepthStencilView * l_pView;
			l_pDeviceContext->OMGetRenderTargets( 1, NULL, &l_pView );
			l_pDeviceContext->OMSetRenderTargets( 1, reinterpret_cast< ID3D11RenderTargetView** >( &m_pOldSurface ), l_pView );
			SafeRelease( l_pView );
		}
		else
		{
			ID3D11RenderTargetView * l_pView;
			l_pDeviceContext->OMGetRenderTargets( 1, &l_pView, NULL );
			l_pDeviceContext->OMSetRenderTargets( 1, &l_pView, reinterpret_cast< ID3D11DepthStencilView* >( m_pOldSurface ) );
			SafeRelease( l_pView );
		}

		l_pDeviceContext->Release();
		SafeRelease( m_pOldSurface );
	}
}

bool DxRenderBuffer :: Resize( Size const & p_size )
{
	return Initialise( p_size );
}

//*************************************************************************************************

DxColourRenderBuffer :: DxColourRenderBuffer( DxRenderSystem * p_pRenderSystem, ePIXEL_FORMAT p_eFormat )
	:	ColourRenderBuffer	( p_eFormat																		)
	,	m_dxRenderBuffer	( p_pRenderSystem, DirectX11::Get( p_eFormat ), eBUFFER_COMPONENT_COLOUR, *this	)
{
}

DxColourRenderBuffer :: ~DxColourRenderBuffer()
{
}

bool DxColourRenderBuffer :: Create()
{
	return m_dxRenderBuffer.Create();
}

void DxColourRenderBuffer :: Destroy()
{
	m_dxRenderBuffer.Destroy();
}

bool DxColourRenderBuffer :: Initialise( Castor::Size const & p_size )
{
	return m_dxRenderBuffer.Initialise( p_size );
}

void DxColourRenderBuffer :: Cleanup()
{
	m_dxRenderBuffer.Cleanup();
}

bool DxColourRenderBuffer :: Bind()
{
	return true;
}

void DxColourRenderBuffer :: Unbind()
{
}

bool DxColourRenderBuffer :: Resize( Castor::Size const & p_size )
{
	return m_dxRenderBuffer.Resize( p_size );
}

//*************************************************************************************************

DxDepthStencilRenderBuffer :: DxDepthStencilRenderBuffer( DxRenderSystem * p_pRenderSystem, ePIXEL_FORMAT p_eFormat )
	:	DepthRenderBuffer	( p_eFormat																		)
	,	m_dxRenderBuffer	( p_pRenderSystem, DirectX11::Get( p_eFormat ), eBUFFER_COMPONENT_DEPTH, *this	)
{
}

DxDepthStencilRenderBuffer :: ~DxDepthStencilRenderBuffer()
{
}

bool DxDepthStencilRenderBuffer :: Create()
{
	return m_dxRenderBuffer.Create();
}

void DxDepthStencilRenderBuffer :: Destroy()
{
	m_dxRenderBuffer.Destroy();
}

bool DxDepthStencilRenderBuffer :: Initialise( Castor::Size const & p_size )
{
	return m_dxRenderBuffer.Initialise( p_size );
}

void DxDepthStencilRenderBuffer :: Cleanup()
{
	m_dxRenderBuffer.Cleanup();
}

bool DxDepthStencilRenderBuffer :: Bind()
{
	return true;
}

void DxDepthStencilRenderBuffer :: Unbind()
{
}

bool DxDepthStencilRenderBuffer :: Resize( Castor::Size const & p_size )
{
	return m_dxRenderBuffer.Resize( p_size );
}


//*************************************************************************************************

DxRenderBufferAttachment :: DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxColourRenderBufferSPtr p_pRenderBuffer )
	:	RenderBufferAttachment	( p_pRenderBuffer						)
	,	m_dxRenderBuffer		( p_pRenderBuffer->GetDxRenderBuffer()	)
	,	m_dwAttachment			( 0xFFFFFFFF							)
	,	m_pRenderSystem			( p_pRenderSystem						)
{
}

DxRenderBufferAttachment :: DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxDepthStencilRenderBufferSPtr p_pRenderBuffer )
	:	RenderBufferAttachment	( p_pRenderBuffer						)
	,	m_dxRenderBuffer		( p_pRenderBuffer->GetDxRenderBuffer()	)
	,	m_dwAttachment			( 0xFFFFFFFF							)
	,	m_pRenderSystem			( p_pRenderSystem						)
{
}

DxRenderBufferAttachment :: ~DxRenderBufferAttachment()
{
}

bool DxRenderBufferAttachment :: DownloadBuffer( PxBufferBaseSPtr p_pBuffer )
{
	return false;
}

bool DxRenderBufferAttachment :: Blit( FrameBufferSPtr p_pBuffer, Rect const & p_rectSrc, Rect const & p_rectDst, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_eInterpolation ) )
{
	return true;
	bool l_bReturn = false;

	DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
	RECT l_rcSrc = { p_rectSrc.left(), p_rectSrc.top(), p_rectSrc.right(), p_rectSrc.bottom() };
	RECT l_rcDst = { p_rectDst.left(), p_rectDst.top(), p_rectDst.right(), p_rectDst.bottom() };
	ID3D11Resource * l_pDstSurface;
	ID3D11Resource * l_pSrcSurface;
	l_pBuffer->GetSurface( GetAttachmentPoint() )->GetResource( &l_pDstSurface );
	m_dxRenderBuffer.GetSurface()->GetResource( &l_pSrcSurface );

	if( l_pDstSurface )
	{
		D3D11_BOX l_box = { 0 };
		l_box.front = 0;
		l_box.back = 1;
		l_box.left = l_rcSrc.left;
		l_box.right = l_rcSrc.right;
		l_box.top = l_rcSrc.top;
		l_box.bottom = l_rcSrc.bottom;
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->CopySubresourceRegion( l_pDstSurface, 0, l_rcDst.left, l_rcDst.top, 0, l_pSrcSurface, 0, &l_box );
		l_pDeviceContext->Release();
	}

	SafeRelease( l_pSrcSurface );
	SafeRelease( l_pDstSurface );

	return l_bReturn;
}

ID3D11View * DxRenderBufferAttachment :: GetSurface()const
{
	return m_dxRenderBuffer.GetSurface();
}

bool DxRenderBufferAttachment :: Bind()
{
	return m_dxRenderBuffer.Bind();
}

void DxRenderBufferAttachment :: Unbind()
{
	m_dxRenderBuffer.Unbind();
}

bool DxRenderBufferAttachment :: DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
{
	m_dwAttachment = DirectX11::Get( p_eAttachment );
	m_pFrameBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pFrameBuffer );
	return true;
}

void DxRenderBufferAttachment :: DoDetach()
{
	Unbind();
}

//*************************************************************************************************

DxTextureAttachment :: DxTextureAttachment( DxRenderSystem * p_pRenderSystem, DynamicTextureSPtr p_pTexture )
	:	TextureAttachment	( p_pTexture													)
	,	m_pOldSurface		( NULL															)
	,	m_dwAttachment		( 0xFFFFFFFF													)
	,	m_pRenderSystem		( p_pRenderSystem												)
	,	m_pDxTexture		( std::static_pointer_cast< DxDynamicTexture >( p_pTexture )	)
{
}

DxTextureAttachment :: ~DxTextureAttachment()
{
	SafeRelease( m_pOldSurface );
}

bool DxTextureAttachment :: DownloadBuffer( PxBufferBaseSPtr p_pBuffer )
{
	bool l_bReturn = false;
// 	D3D11_MAPPED_SUBRESOURCE l_mappedResource;
// 	ID3D11Resource * l_pResource;
// 	ID3D11RenderTargetView * l_pSurface = m_pDxTexture.lock()->GetRenderTargetView();
// 	l_pSurface->GetResource( &l_pResource );
// 	HRESULT l_hr = m_pRenderSystem->GetDeviceContext()->Map( l_pResource, 0, D3D11_MAP_READ, 0, &l_mappedResource );
// 
// 	if( l_hr == S_OK && l_mappedResource.pData != NULL )
// 	{
// 		l_bReturn = true;
// 		std::memcpy( p_pBuffer->ptr(), l_mappedResource.pData, p_pBuffer->size() );
// 	}

	return l_bReturn;
}

bool DxTextureAttachment :: Blit( FrameBufferSPtr p_pBuffer, Rect const & p_rectSrc, Rect const & p_rectDst, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_eInterpolation ) )
{
	bool l_bReturn = true;

	DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
	RECT l_rcSrc = { p_rectSrc.left(), p_rectSrc.top(), p_rectSrc.right(), p_rectSrc.bottom() };
	RECT l_rcDst = { p_rectDst.left(), p_rectDst.top(), p_rectDst.right(), p_rectDst.bottom() };
	ID3D11Resource * l_pDstSurface;
	ID3D11Resource * l_pSrcSurface;
	l_pBuffer->GetSurface( GetAttachmentPoint() )->GetResource( &l_pDstSurface );
	m_pDxTexture.lock()->GetShaderResourceView()->GetResource( &l_pSrcSurface );

	if( l_pDstSurface )
	{
		D3D11_BOX l_box = { 0 };
		l_box.front = 0;
		l_box.back = 1;
		l_box.left = l_rcSrc.left;
		l_box.right = l_rcSrc.right;
		l_box.top = l_rcSrc.top;
		l_box.bottom = l_rcSrc.bottom;
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->CopySubresourceRegion( l_pDstSurface, 0, l_rcDst.left, l_rcDst.top, 0, l_pSrcSurface, 0, &l_box );
		l_pDeviceContext->Release();
	}

	SafeRelease( l_pSrcSurface );
	SafeRelease( l_pDstSurface );

	return l_bReturn;
}

ID3D11View * DxTextureAttachment :: GetSurface()const
{
	return NULL;
//	return m_pDxTexture.lock()->GetDxSurface();
}

bool DxTextureAttachment :: Bind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	ID3D11DepthStencilView * l_pView;
	ID3D11RenderTargetView * l_pSurface = m_pDxTexture.lock()->GetRenderTargetView();
	l_pDeviceContext->OMGetRenderTargets( 1, &m_pOldSurface, &l_pView );
	l_pDeviceContext->OMSetRenderTargets( 1, &l_pSurface, l_pView );
	SafeRelease( l_pView );
	l_pDeviceContext->Release();
	return true;
}

void DxTextureAttachment :: Unbind()
{
	if( m_pOldSurface )
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		ID3D11DepthStencilView * l_pView;
		l_pDeviceContext->OMGetRenderTargets( 1, NULL, &l_pView );
		l_pDeviceContext->OMSetRenderTargets( 1, &m_pOldSurface, l_pView );
		SafeRelease( l_pView );
		SafeRelease( m_pOldSurface );
		l_pDeviceContext->Release();
	}
}

bool DxTextureAttachment :: DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
{
	m_dwAttachment = DirectX11::Get( p_eAttachment );
	m_pFrameBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pFrameBuffer );

	return true;
}

void DxTextureAttachment :: DoDetach()
{
	Unbind();
}

//*************************************************************************************************

DxFrameBuffer :: DxFrameBuffer( DxRenderSystem * p_pRenderSystem )
	:	FrameBuffer				( p_pRenderSystem->GetEngine()	)
	,	m_pRenderSystem			( p_pRenderSystem				)
	,	m_pOldDepthStencilView	( NULL							)
{
}

DxFrameBuffer :: ~DxFrameBuffer()
{
}

bool DxFrameBuffer :: Create( int CU_PARAM_UNUSED( p_iSamplesCount ) )
{
	return true;
}

void DxFrameBuffer :: Destroy()
{
	SafeRelease( m_pOldDepthStencilView );
}

bool DxFrameBuffer :: SetDrawBuffers( uint32_t CU_PARAM_UNUSED( p_uiSize ), eATTACHMENT_POINT const * CU_PARAM_UNUSED( p_eAttaches ) )
{
	return SetDrawBuffers();
}

bool DxFrameBuffer :: SetDrawBuffers()
{
	bool l_bReturn = false;

	if( m_mapRbo.size() || m_mapTex.size() )
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		D3D11RenderTargetViewArray l_arraySurfaces;
		ID3D11DepthStencilView * l_pView = NULL;
		l_arraySurfaces.reserve( 10 );

		for( TexAttachMapIt l_it = m_mapTex.begin(); l_it != m_mapTex.end(); ++l_it )
		{
			if( l_it->first != eATTACHMENT_POINT_DEPTH && l_it->first != eATTACHMENT_POINT_STENCIL && l_it->first != eATTACHMENT_POINT_NONE )
			{
				l_arraySurfaces.push_back( std::static_pointer_cast< DxDynamicTexture >( l_it->second )->GetRenderTargetView() );
			}
		}

		for( RboAttachMapIt l_it = m_mapRbo.begin(); l_it != m_mapRbo.end(); ++l_it )
		{
			if( l_it->first == eATTACHMENT_POINT_DEPTH || l_it->first == eATTACHMENT_POINT_STENCIL )
			{
				l_pView = reinterpret_cast< ID3D11DepthStencilView* >( std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface() );
			}
			else if( l_it->first != eATTACHMENT_POINT_NONE )
			{
				l_arraySurfaces.push_back( reinterpret_cast< ID3D11RenderTargetView* >( std::static_pointer_cast< DxColourRenderBuffer >( l_it->second )->GetDxRenderBuffer().GetSurface() ) );
			}
		}

		if( l_arraySurfaces.size() )
		{
			m_arrayOldRenderTargets.resize( l_arraySurfaces.size() );
			l_pDeviceContext->OMGetRenderTargets( UINT( l_arraySurfaces.size() ), m_arrayOldRenderTargets.data(), &m_pOldDepthStencilView );
			l_pDeviceContext->OMSetRenderTargets( UINT( l_arraySurfaces.size() ), &l_arraySurfaces[0], l_pView );
			l_bReturn = true;
		}
		else if( l_pView )
		{
			l_pDeviceContext->OMGetRenderTargets( 0, NULL, &m_pOldDepthStencilView );
			l_pDeviceContext->OMSetRenderTargets( 0, NULL, l_pView );
		}

		l_pDeviceContext->Release();
	}

	return l_bReturn;
}

ColourRenderBufferSPtr DxFrameBuffer :: CreateColourRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
{
	return std::make_shared< DxColourRenderBuffer >( m_pRenderSystem, p_ePixelFormat );
}

DepthRenderBufferSPtr DxFrameBuffer :: CreateDepthRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
{
	return std::make_shared< DxDepthStencilRenderBuffer >( m_pRenderSystem, p_ePixelFormat );
}

StencilRenderBufferSPtr DxFrameBuffer :: CreateStencilRenderBuffer( ePIXEL_FORMAT CU_PARAM_UNUSED( p_ePixelFormat ) )
{
	return nullptr;
}

ID3D11View * DxFrameBuffer :: GetSurface( Castor3D::eATTACHMENT_POINT p_eAttach )
{
	ID3D11View * l_pReturn = NULL;

	if( m_mapRbo.size() || m_mapTex.size() && p_eAttach != eATTACHMENT_POINT_NONE )
	{
		if( p_eAttach != eATTACHMENT_POINT_STENCIL && p_eAttach != eATTACHMENT_POINT_DEPTH )
		{
			for( TexAttachMapIt l_it = m_mapTex.begin(); l_it != m_mapTex.end() && !l_pReturn; ++l_it )
			{
				if( l_it->first == p_eAttach )
				{
					l_pReturn = std::static_pointer_cast< DxDynamicTexture >( l_it->second )->GetRenderTargetView();
				}
			}
		}

		for( RboAttachMapIt l_it = m_mapRbo.begin(); l_it != m_mapRbo.end() && !l_pReturn; ++l_it )
		{
			if( l_it->first == eATTACHMENT_POINT_DEPTH || l_it->first == eATTACHMENT_POINT_STENCIL )
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

bool DxFrameBuffer :: DoBind( eFRAMEBUFFER_TARGET CU_PARAM_UNUSED( p_eTarget ) )
{
	return true;
}

void DxFrameBuffer :: DoUnbind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	D3D11RenderTargetViewArray l_arraySurfaces;
	ID3D11DepthStencilView * l_pView = m_pOldDepthStencilView;

	for( std::size_t i = 0; i < m_arrayOldRenderTargets.size(); ++i )
	{
		if( m_arrayOldRenderTargets[i] )
		{
			l_arraySurfaces.push_back( m_arrayOldRenderTargets[i] );
		}
	}

	if( l_pView || !l_arraySurfaces.empty() )
	{
		l_pDeviceContext->OMSetRenderTargets( UINT( l_arraySurfaces.size() ), l_arraySurfaces.empty() ? NULL : l_arraySurfaces.data(), l_pView );
	}

	for( std::size_t i = 0; i < l_arraySurfaces.size(); ++i )
	{
		SafeRelease( l_arraySurfaces[i] );
	}

	l_pDeviceContext->Release();
	SafeRelease( m_pOldDepthStencilView );
}

void DxFrameBuffer :: DoAttachFba( Castor3D::FrameBufferAttachmentRPtr CU_PARAM_UNUSED( p_pAttach ) )
{
}

void DxFrameBuffer :: DoDetachFba( Castor3D::FrameBufferAttachmentRPtr CU_PARAM_UNUSED( p_pAttach ) )
{
}

bool DxFrameBuffer :: DoAttach( eATTACHMENT_POINT CU_PARAM_UNUSED( p_eAttachment ), DynamicTextureSPtr CU_PARAM_UNUSED( p_pTexture ), eTEXTURE_TARGET CU_PARAM_UNUSED( p_eTarget ), int CU_PARAM_UNUSED( p_iLayer ) )
{
	return true;
}

bool DxFrameBuffer :: DoAttach( eATTACHMENT_POINT CU_PARAM_UNUSED( p_eAttachment ), RenderBufferSPtr CU_PARAM_UNUSED( p_pRenderBuffer ) )
{
	return true;
}

void DxFrameBuffer :: DoDetachAll()
{
}

bool DxFrameBuffer :: DoStretchInto( FrameBufferSPtr p_pBuffer, Castor::Rect const & p_rectSrc, Castor::Rect const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_eInterpolation ) )
{
	SrcDstPairArray l_arrayPairs;
	DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
	bool l_bDepth = (p_uiComponents & eBUFFER_COMPONENT_DEPTH) == eBUFFER_COMPONENT_DEPTH;
	bool l_bStencil = (p_uiComponents & eBUFFER_COMPONENT_STENCIL) == eBUFFER_COMPONENT_STENCIL;
	bool l_bColour = (p_uiComponents & eBUFFER_COMPONENT_COLOUR) == eBUFFER_COMPONENT_COLOUR;

	for( AttachArrayConstIt l_it = m_arrayAttaches.begin(); l_it != m_arrayAttaches.end(); ++l_it )
	{
		eATTACHMENT_POINT l_eAttach = *l_it;
		if( (l_eAttach != eATTACHMENT_POINT_DEPTH && l_eAttach != eATTACHMENT_POINT_STENCIL) || m_pRenderSystem->GetFeatureLevel() > D3D_FEATURE_LEVEL_10_1 )
		{
			if( (l_bDepth && l_eAttach == eATTACHMENT_POINT_DEPTH) || (l_bStencil && l_eAttach == eATTACHMENT_POINT_STENCIL) || (l_bColour && l_eAttach != eATTACHMENT_POINT_DEPTH && l_eAttach != eATTACHMENT_POINT_STENCIL) )
			{
				ID3D11View * l_pSrc = GetSurface( *l_it );
				ID3D11View * l_pDst = l_pBuffer->GetSurface( *l_it );

				if( l_pSrc && l_pDst )
				{
					l_arrayPairs.push_back( std::make_pair( l_pSrc, l_pDst ) );
				}
			}
		}
	}

	HRESULT l_hr = S_OK;
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );

	for( SrcDstPairArrayIt l_itArray = l_arrayPairs.begin(); l_itArray != l_arrayPairs.end() && l_hr == S_OK; ++l_itArray )
	{
		try
		{
			ID3D11Resource * l_pDstSurface;
			ID3D11Resource * l_pSrcSurface;

			l_itArray->first->GetResource( &l_pDstSurface );
			l_itArray->second->GetResource( &l_pSrcSurface );

			if( l_pDstSurface && l_pSrcSurface )
			{
				D3D11_BOX l_box = { 0 };
				l_box.front = 0;
				l_box.back = 1;
				l_box.left = p_rectSrc.left();
				l_box.right = p_rectSrc.right();
				l_box.top = p_rectSrc.top();
				l_box.bottom = p_rectSrc.bottom();
				l_pDeviceContext->CopySubresourceRegion( l_pDstSurface, 0, p_rectDst.left(), p_rectDst.top(), 0, l_pSrcSurface, 0, NULL );//&l_box );
			}

			SafeRelease( l_pSrcSurface );
			SafeRelease( l_pDstSurface );
		}
		catch( ... )
		{
			Logger::LogError( cuT( "Error while stretching src to dst frame buffer" ) );
			l_hr = E_FAIL;
		}
	}

	l_pDeviceContext->Release();

	return l_hr == S_OK;
}

//*************************************************************************************************
