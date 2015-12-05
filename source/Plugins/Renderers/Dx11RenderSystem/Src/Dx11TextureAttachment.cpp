#include "Dx11TextureAttachment.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11DynamicTexture.hpp"
#include "Dx11FrameBuffer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxTextureAttachment::DxTextureAttachment( DxRenderSystem * p_renderSystem, DynamicTextureSPtr p_texture )
		: TextureAttachment( p_texture )
		, m_renderSystem( p_renderSystem )
		, m_pDxTexture( std::static_pointer_cast< DxDynamicTexture >( p_texture ) )
	{
	}

	DxTextureAttachment::~DxTextureAttachment()
	{
	}

	bool DxTextureAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_interpolation ) )
	{
		bool l_return = true;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_buffer );
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
			ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
			l_deviceContext->CopySubresourceRegion( l_pDstSurface, 0, l_rcDst.left, l_rcDst.top, 0, l_pSrcSurface, 0, &l_box );
		}

		SafeRelease( l_pSrcSurface );
		SafeRelease( l_pDstSurface );
		return l_return;
	}

	bool DxTextureAttachment::DoAttach( FrameBufferSPtr p_frameBuffer )
	{
		return true;
	}

	void DxTextureAttachment::DoDetach()
	{
	}
}
