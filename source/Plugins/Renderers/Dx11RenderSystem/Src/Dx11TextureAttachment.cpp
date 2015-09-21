#include "Dx11TextureAttachment.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11DynamicTexture.hpp"
#include "Dx11FrameBuffer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxTextureAttachment::DxTextureAttachment( DxRenderSystem * p_pRenderSystem, DynamicTextureSPtr p_pTexture )
		: TextureAttachment( p_pTexture )
		, m_pOldSurface( NULL )
		, m_dwAttachment( 0xFFFFFFFF )
		, m_pRenderSystem( p_pRenderSystem )
		, m_pDxTexture( std::static_pointer_cast< DxDynamicTexture >( p_pTexture ) )
	{
	}

	DxTextureAttachment::~DxTextureAttachment()
	{
		SafeRelease( m_pOldSurface );
	}

	bool DxTextureAttachment::DownloadBuffer( PxBufferBaseSPtr p_pBuffer )
	{
		bool l_return = false;
		//D3D11_MAPPED_SUBRESOURCE l_mappedResource;
		//ID3D11Resource * l_pResource;
		//ID3D11RenderTargetView * l_pSurface = m_pDxTexture.lock()->GetRenderTargetView();
		//l_pSurface->GetResource( &l_pResource );
		//HRESULT l_hr = m_pRenderSystem->GetDeviceContext()->Map( l_pResource, 0, D3D11_MAP_READ, 0, &l_mappedResource );
		//
		//if( l_hr == S_OK && l_mappedResource.pData != NULL )
		//{
		//	l_return = true;
		//	std::memcpy( p_pBuffer->ptr(), l_mappedResource.pData, p_pBuffer->size() );
		//}
		return l_return;
	}

	bool DxTextureAttachment::Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_eInterpolation ) )
	{
		bool l_return = true;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
		RECT l_rcSrc = { p_rectSrc.left(), p_rectSrc.top(), p_rectSrc.right(), p_rectSrc.bottom() };
		RECT l_rcDst = { p_rectDst.left(), p_rectDst.top(), p_rectDst.right(), p_rectDst.bottom() };
		ID3D11Resource * l_pDstSurface;
		ID3D11Resource * l_pSrcSurface;
		l_pBuffer->GetSurface( GetAttachmentPoint() )->GetResource( &l_pDstSurface );
		m_pDxTexture.lock()->GetShaderResourceView()->GetResource( &l_pSrcSurface );

		if ( l_pDstSurface && l_pSrcSurface )
		{
			D3D11_BOX l_box = { 0 };
			l_box.front = 0;
			l_box.back = 1;
			l_box.left = l_rcSrc.left;
			l_box.right = l_rcSrc.right;
			l_box.top = l_rcSrc.top;
			l_box.bottom = l_rcSrc.bottom;
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
			l_pDeviceContext->CopySubresourceRegion( l_pDstSurface, 0, l_rcDst.left, l_rcDst.top, 0, l_pSrcSurface, 0, &l_box );
		}

		SafeRelease( l_pSrcSurface );
		SafeRelease( l_pDstSurface );
		return l_return;
	}

	ID3D11View * DxTextureAttachment::GetSurface()const
	{
		return NULL;
		//	return m_pDxTexture.lock()->GetDxSurface();
	}

	bool DxTextureAttachment::DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		m_dwAttachment = DirectX11::Get( p_eAttachment );
		m_pFrameBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pFrameBuffer );
		return true;
	}

	void DxTextureAttachment::DoDetach()
	{
		m_pFrameBuffer.reset();
		m_dwAttachment = 0;
	}
}
