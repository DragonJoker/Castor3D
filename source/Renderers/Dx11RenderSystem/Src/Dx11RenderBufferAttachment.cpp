#include "Dx11RenderBufferAttachment.hpp"
#include "Dx11ColourRenderBuffer.hpp"
#include "Dx11DepthStencilRenderBuffer.hpp"
#include "Dx11FrameBuffer.hpp"
#include "Dx11RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxRenderBufferAttachment::DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxColourRenderBufferSPtr p_pRenderBuffer )
		: RenderBufferAttachment( p_pRenderBuffer )
		, m_dxRenderBuffer( p_pRenderBuffer->GetDxRenderBuffer() )
		, m_dwAttachment( 0xFFFFFFFF )
		, m_pRenderSystem( p_pRenderSystem )
	{
	}

	DxRenderBufferAttachment::DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxDepthStencilRenderBufferSPtr p_pRenderBuffer )
		: RenderBufferAttachment( p_pRenderBuffer )
		, m_dxRenderBuffer( p_pRenderBuffer->GetDxRenderBuffer() )
		, m_dwAttachment( 0xFFFFFFFF )
		, m_pRenderSystem( p_pRenderSystem )
	{
	}

	DxRenderBufferAttachment::~DxRenderBufferAttachment()
	{
	}

	bool DxRenderBufferAttachment::DownloadBuffer( PxBufferBaseSPtr p_pBuffer )
	{
		return false;
	}

	bool DxRenderBufferAttachment::Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_eInterpolation ) )
	{
		bool l_bReturn = false;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
		RECT l_rcSrc = { p_rectSrc.left(), p_rectSrc.top(), p_rectSrc.right(), p_rectSrc.bottom() };
		RECT l_rcDst = { p_rectDst.left(), p_rectDst.top(), p_rectDst.right(), p_rectDst.bottom() };
		ID3D11Resource * l_pDstSurface;
		ID3D11Resource * l_pSrcSurface;
		l_pBuffer->GetSurface( GetAttachmentPoint() )->GetResource( &l_pDstSurface );
		m_dxRenderBuffer.GetSurface()->GetResource( &l_pSrcSurface );

		if ( l_pDstSurface )
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
		return l_bReturn;
	}

	ID3D11View * DxRenderBufferAttachment::GetSurface()const
	{
		return m_dxRenderBuffer.GetSurface();
	}

	bool DxRenderBufferAttachment::Bind()
	{
		return m_dxRenderBuffer.Bind();
	}

	void DxRenderBufferAttachment::Unbind()
	{
		m_dxRenderBuffer.Unbind();
	}

	bool DxRenderBufferAttachment::DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		m_dwAttachment = DirectX11::Get( p_eAttachment );
		m_pFrameBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pFrameBuffer );
		return true;
	}

	void DxRenderBufferAttachment::DoDetach()
	{
		Unbind();
	}
}
