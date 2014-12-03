#include "Dx9TextureAttachment.hpp"
#include "Dx9FrameBuffer.hpp"
#include "Dx9RenderSystem.hpp"
#include "Dx9DynamicTexture.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	DxTextureAttachment::DxTextureAttachment( DxRenderSystem * p_pRenderSystem, DynamicTextureSPtr p_pTexture )
		:	TextureAttachment( p_pTexture )
		,	m_pOldSurface( NULL )
		,	m_dwAttachment( 0xFFFFFFFF )
		,	m_pDevice( NULL )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pDxTexture( std::static_pointer_cast< DxDynamicTexture >( p_pTexture ) )
	{
	}

	DxTextureAttachment::~DxTextureAttachment()
	{
		SafeRelease( m_pOldSurface );
	}

	bool DxTextureAttachment::DownloadBuffer( PxBufferBaseSPtr p_pBuffer )
	{
		return false;
	}

	bool DxTextureAttachment::Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_eInterpolation )
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

	IDirect3DSurface9 * DxTextureAttachment::GetSurface()const
	{
		return m_pDxTexture->GetDxSurface();
	}

	bool DxTextureAttachment::Bind()
	{
		HRESULT l_hr = S_FALSE;

		if ( m_pDxTexture && m_pDxTexture->GetDxSurface() )
		{
			l_hr = m_pDevice->GetRenderTarget( m_dwAttachment, &m_pOldSurface );

			if ( l_hr == S_OK )
			{
				l_hr = m_pDevice->SetRenderTarget( m_dwAttachment, m_pDxTexture->GetDxSurface() );
			}
		}

		return l_hr == S_OK;
	}

	void DxTextureAttachment::Unbind()
	{
		if ( m_pOldSurface )
		{
			m_pDevice->SetRenderTarget( m_dwAttachment, m_pOldSurface );
			SafeRelease( m_pOldSurface );
		}
	}

	bool DxTextureAttachment::DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		Logger::LogDebug( L"Attaching Texture [0x%08X] to FrameBuffer [0x%08X]", GetTexture().get(), p_pFrameBuffer.get() );
		m_dwAttachment = DirectX9::Get( p_eAttachment );
		m_pFrameBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pFrameBuffer );

		if ( !m_pDevice )
		{
			m_pDevice = m_pRenderSystem->GetDevice();
		}

		return true;
	}

	void DxTextureAttachment::DoDetach()
	{
		Unbind();
	}
}
