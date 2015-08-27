#include "Dx9RenderBufferAttachment.hpp"
#include "Dx9ColourRenderBuffer.hpp"
#include "Dx9DepthStencilRenderBuffer.hpp"
#include "Dx9FrameBuffer.hpp"
#include "Dx9RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	DxRenderBufferAttachment::DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxColourRenderBufferSPtr p_pRenderBuffer )
		:	RenderBufferAttachment( p_pRenderBuffer )
		,	m_dxRenderBuffer( p_pRenderBuffer->GetDxRenderBuffer() )
		,	m_dwAttachment( 0xFFFFFFFF )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pDevice( NULL )
	{
	}

	DxRenderBufferAttachment::DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxDepthStencilRenderBufferSPtr p_pRenderBuffer )
		:	RenderBufferAttachment( p_pRenderBuffer )
		,	m_dxRenderBuffer( p_pRenderBuffer->GetDxRenderBuffer() )
		,	m_dwAttachment( 0xFFFFFFFF )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pDevice( NULL )
	{
	}

	DxRenderBufferAttachment::~DxRenderBufferAttachment()
	{
	}

	bool DxRenderBufferAttachment::DownloadBuffer( PxBufferBaseSPtr p_pBuffer )
	{
		return false;
	}

	bool DxRenderBufferAttachment::Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_eInterpolation )
	{
		bool l_bReturn = false;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
		RECT l_rcSrc = { p_rectSrc.left(), p_rectSrc.top(), p_rectSrc.right(), p_rectSrc.bottom() };
		RECT l_rcDst = { p_rectDst.left(), p_rectDst.top(), p_rectDst.right(), p_rectDst.bottom() };
		IDirect3DSurface9 * l_pSurface = l_pBuffer->GetSurface( GetAttachmentPoint() );

		if ( l_pSurface )
		{
			l_bReturn = m_pDevice->StretchRect( GetSurface(), &l_rcSrc, l_pBuffer->GetSurface( GetAttachmentPoint() ), &l_rcDst, D3DTEXTUREFILTERTYPE( DirectX9::Get( p_eInterpolation ) ) ) == S_OK;
		}

		return l_bReturn;
	}

	IDirect3DSurface9 * DxRenderBufferAttachment::GetSurface()const
	{
		return m_dxRenderBuffer.GetSurface();
	}

	bool DxRenderBufferAttachment::Bind()
	{
		if ( !m_pDevice )
		{
			m_pDevice = m_pRenderSystem->GetDevice();
		}

		return m_dxRenderBuffer.Bind( m_dwAttachment );
	}

	void DxRenderBufferAttachment::Unbind()
	{
		m_dxRenderBuffer.Unbind( m_dwAttachment );
	}

	bool DxRenderBufferAttachment::DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		Logger::LogDebug( L"Attaching RenderBuffer [0x%08X] to FrameBuffer [0x%08X]", GetRenderBuffer().get(), p_pFrameBuffer.get() );
		m_dwAttachment = DirectX9::Get( p_eAttachment );
		m_pFrameBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pFrameBuffer );
		return true;
	}

	void DxRenderBufferAttachment::DoDetach()
	{
		Unbind();
	}
}
