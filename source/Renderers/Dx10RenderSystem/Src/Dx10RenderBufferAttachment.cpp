#include "Dx10RenderBufferAttachment.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10ColourRenderBuffer.hpp"
#include "Dx10DepthStencilRenderBuffer.hpp"
#include "Dx10FrameBuffer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
{
	DxRenderBufferAttachment::DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxColourRenderBufferSPtr p_pRenderBuffer )
		: RenderBufferAttachment( p_pRenderBuffer )
		, m_dxRenderBuffer( p_pRenderBuffer->GetDxRenderBuffer() )
		, m_dwAttachment( 0xFFFFFFFF )
		, m_pRenderSystem( p_pRenderSystem )
		, m_pDevice( NULL )
	{
	}

	DxRenderBufferAttachment::DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxDepthStencilRenderBufferSPtr p_pRenderBuffer )
		: RenderBufferAttachment( p_pRenderBuffer )
		, m_dxRenderBuffer( p_pRenderBuffer->GetDxRenderBuffer() )
		, m_dwAttachment( 0xFFFFFFFF )
		, m_pRenderSystem( p_pRenderSystem )
		, m_pDevice( NULL )
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
#if 1
		return true;
#else
		bool l_bReturn = false;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
		RECT l_rcSrc = { p_rectSrc.left(), p_rectSrc.top(), p_rectSrc.right(), p_rectSrc.bottom() };
		RECT l_rcDst = { p_rectDst.left(), p_rectDst.top(), p_rectDst.right(), p_rectDst.bottom() };
		ID3D10Resource * l_pDstSurface;
		ID3D10Resource * l_pSrcSurface;
		l_pBuffer->GetSurface( GetAttachmentPoint() )->GetResource( &l_pDstSurface );
		m_dxRenderBuffer.GetSurface()->GetResource( &l_pSrcSurface );

		if ( l_pDstSurface )
		{
			D3D10_BOX l_box = { 0 };
			l_box.front = 0;
			l_box.back = 1;
			l_box.left = l_rcSrc.left;
			l_box.right = l_rcSrc.right;
			l_box.top = l_rcSrc.top;
			l_box.bottom = l_rcSrc.bottom;
			m_pRenderSystem->GetDevice()->CopySubresourceRegion( l_pDstSurface, 0, l_rcDst.left, l_rcDst.top, 0, l_pSrcSurface, 0, &l_box );
		}

		SafeRelease( l_pSrcSurface );
		SafeRelease( l_pDstSurface );
		return l_bReturn;
#endif
	}

	ID3D10View * DxRenderBufferAttachment::GetSurface()const
	{
		return m_dxRenderBuffer.GetSurface();
	}

	bool DxRenderBufferAttachment::Bind()
	{
		if ( !m_pDevice )
		{
			m_pDevice = m_pRenderSystem->GetDevice();
		}

		return m_dxRenderBuffer.Bind();
	}

	void DxRenderBufferAttachment::Unbind()
	{
		m_dxRenderBuffer.Unbind();
	}

	bool DxRenderBufferAttachment::DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		m_dwAttachment = DirectX10::Get( p_eAttachment );
		m_pFrameBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pFrameBuffer );
		return true;
	}

	void DxRenderBufferAttachment::DoDetach()
	{
		Unbind();
	}
}
