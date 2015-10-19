#include "Dx11FrameBuffer.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11DynamicTexture.hpp"
#include "Dx11ColourRenderBuffer.hpp"
#include "Dx11DepthStencilRenderBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxFrameBuffer::DxFrameBuffer( DxRenderSystem * p_renderSystem )
		: FrameBuffer( *p_renderSystem->GetOwner() )
		, m_renderSystem( p_renderSystem )
		, m_pOldDepthStencilView( NULL )
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
	}

	bool DxFrameBuffer::SetDrawBuffers( AttachArray const & p_attaches )
	{
		bool l_return = false;

		if ( !p_attaches.empty() )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
			D3D11RenderTargetViewArray l_arraySurfaces;
			ID3D11DepthStencilView * l_pView = NULL;
			l_arraySurfaces.reserve( p_attaches.size() );

			for ( auto && l_attach : p_attaches )
			{
				eATTACHMENT_POINT l_eAttach = l_attach->GetAttachmentPoint();

				if ( l_attach->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
				{
					TextureAttachmentSPtr l_texAttach = std::static_pointer_cast< TextureAttachment >( l_attach );

					if ( l_eAttach = eATTACHMENT_POINT_COLOUR )
					{
						l_arraySurfaces.push_back( std::static_pointer_cast< DxDynamicTexture >( l_texAttach->GetTexture() )->GetRenderTargetView() );
					}
				}
				else
				{
					RenderBufferAttachmentSPtr l_rboAttach = std::static_pointer_cast< RenderBufferAttachment >( l_attach );

					if ( l_eAttach == eATTACHMENT_POINT_DEPTH || l_eAttach == eATTACHMENT_POINT_STENCIL )
					{
						l_pView = reinterpret_cast< ID3D11DepthStencilView * >( std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_rboAttach->GetRenderBuffer() )->GetDxRenderBuffer().GetResourceView() );
					}
					else if ( l_eAttach == eATTACHMENT_POINT_COLOUR )
					{
						l_arraySurfaces.push_back( reinterpret_cast< ID3D11RenderTargetView * >( std::static_pointer_cast< DxColourRenderBuffer >( l_rboAttach->GetRenderBuffer() )->GetDxRenderBuffer().GetResourceView() ) );
					}
				}
			}

			if ( l_arraySurfaces.size() )
			{
				l_pDeviceContext->OMSetRenderTargets( UINT( l_arraySurfaces.size() ), &l_arraySurfaces[0], l_pView );
				l_return = true;
			}
			else if ( l_pView )
			{
				l_pDeviceContext->OMSetRenderTargets( 0, NULL, l_pView );
			}
		}

		return l_return;
	}

	ColourRenderBufferSPtr DxFrameBuffer::CreateColourRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< DxColourRenderBuffer >( m_renderSystem, p_ePixelFormat );
	}

	DepthStencilRenderBufferSPtr DxFrameBuffer::CreateDepthStencilRenderBuffer( ePIXEL_FORMAT p_ePixelFormat )
	{
		return std::make_shared< DxDepthStencilRenderBuffer >( m_renderSystem, p_ePixelFormat );
	}

	bool DxFrameBuffer::DownloadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		DxDynamicTextureSPtr l_texture;

		if ( p_point == eATTACHMENT_POINT_COLOUR )
		{
			l_texture = m_colorBuffer;
		}
		else
		{
			l_texture = m_depthBuffer;
		}

		auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point, &p_index]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentPoint() == p_point && p_attach->GetAttachmentIndex() == p_index;
		} );

		bool l_return = l_it != m_attaches.end();

		if ( l_return )
		{
			ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
			FrameBufferAttachmentSPtr l_attach = *l_it;
			ID3D11Resource * l_src = NULL;

			switch ( l_attach->GetAttachmentType() )
			{
			case eATTACHMENT_TYPE_TEXTURE:
				l_src = std::static_pointer_cast< DxDynamicTexture >( std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture() )->GetTexture2D();
				break;

			case eATTACHMENT_TYPE_BUFFER:
				if ( p_point != eATTACHMENT_POINT_COLOUR )
				{
					l_src = std::static_pointer_cast< DxDepthStencilRenderBuffer >( std::static_pointer_cast< RenderBufferAttachment >( l_attach )->GetRenderBuffer() )->GetDxRenderBuffer().GetResource();
				}
				else
				{
					l_src = std::static_pointer_cast< DxColourRenderBuffer >( std::static_pointer_cast< RenderBufferAttachment >( l_attach )->GetRenderBuffer() )->GetDxRenderBuffer().GetResource();
				}
				break;
			}

			l_deviceContext->CopyResource( l_texture->GetTexture2D(), l_src );
			uint8_t * l_buffer = l_texture->Lock( eACCESS_TYPE_READ );

			if ( l_buffer )
			{
				p_buffer->assign( std::vector< uint8_t >( l_buffer, l_buffer + l_texture->GetBuffer()->size() ), l_texture->GetPixelFormat() );
				l_texture->Unlock( false );
			}
		}

		return l_return;
	}

	ID3D11View * DxFrameBuffer::GetSurface( Castor3D::eATTACHMENT_POINT p_eAttach )
	{
		ID3D11View * l_return = NULL;

		if ( !m_attaches.empty() && p_eAttach != eATTACHMENT_POINT_NONE )
		{
			auto && l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_eAttach]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->GetAttachmentPoint() == p_eAttach;
			} );

			if ( l_it != m_attaches.end() )
			{
				if ( ( *l_it )->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
				{
					TextureAttachmentSPtr l_attach = std::static_pointer_cast< TextureAttachment >( *l_it );
					l_return = std::static_pointer_cast< DxDynamicTexture >( l_attach->GetTexture() )->GetRenderTargetView();
				}
				else
				{
					RenderBufferAttachmentSPtr l_attach = std::static_pointer_cast< RenderBufferAttachment >( *l_it );
					eATTACHMENT_POINT l_eAttach = l_attach->GetAttachmentPoint();

					if ( l_eAttach == eATTACHMENT_POINT_DEPTH || l_eAttach == eATTACHMENT_POINT_STENCIL )
					{
						l_return = std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_attach->GetRenderBuffer() )->GetDxRenderBuffer().GetResourceView();
					}
					else
					{
						l_return = std::static_pointer_cast< DxColourRenderBuffer >( l_attach->GetRenderBuffer() )->GetDxRenderBuffer().GetResourceView();
					}
				}
			}
		}

		return l_return;
	}

	bool DxFrameBuffer::DoInitialise( Castor::Size const & p_size )
	{
		m_colorBuffer = std::static_pointer_cast< DxDynamicTexture >( GetOwner()->GetRenderSystem()->CreateDynamicTexture() );
		bool l_return = m_colorBuffer != nullptr;

		if ( l_return )
		{
			m_colorBuffer->SetType( eTEXTURE_TYPE_2D );
			m_colorBuffer->SetImage( p_size, ePIXEL_FORMAT_A8B8G8R8 );
			l_return = m_colorBuffer->Create();
		}

		if ( l_return )
		{
			m_colorBuffer->Initialise( 0, eACCESS_TYPE_READ, eACCESS_TYPE_WRITE );
		}

		m_depthBuffer = std::static_pointer_cast< DxDynamicTexture >( GetOwner()->GetRenderSystem()->CreateDynamicTexture() );
		l_return = m_depthBuffer != nullptr;

		if ( l_return )
		{
			m_depthBuffer->SetType( eTEXTURE_TYPE_2D );
			m_depthBuffer->SetImage( p_size, ePIXEL_FORMAT_DEPTH24S8 );
			l_return = m_depthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_depthBuffer->Initialise( 0, eACCESS_TYPE_READ, eACCESS_TYPE_WRITE );
		}

		return l_return;
	}

	void DxFrameBuffer::DoCleanup()
	{
		m_depthBuffer->Cleanup();
		m_depthBuffer->Destroy();
		m_depthBuffer.reset();

		m_colorBuffer->Cleanup();
		m_colorBuffer->Destroy();
		m_colorBuffer.reset();
	}

	bool DxFrameBuffer::DoBind( eFRAMEBUFFER_TARGET p_eTarget )
	{
		return true;
	}

	void DxFrameBuffer::DoUnbind()
	{
	}

	void DxFrameBuffer::DoResize( Castor::Size const & p_size )
	{
		m_colorBuffer->Resize( p_size );
		m_depthBuffer->Resize( p_size );
	}

	bool DxFrameBuffer::DoBlitInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_eInterpolation ) )
	{
		SrcDstPairArray l_arrayPairs;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_pBuffer );
		bool l_bDepth = ( p_uiComponents & eBUFFER_COMPONENT_DEPTH ) == eBUFFER_COMPONENT_DEPTH;
		bool l_bStencil = ( p_uiComponents & eBUFFER_COMPONENT_STENCIL ) == eBUFFER_COMPONENT_STENCIL;
		bool l_bColour = ( p_uiComponents & eBUFFER_COMPONENT_COLOUR ) == eBUFFER_COMPONENT_COLOUR;

		for ( auto && l_attach : m_attaches )
		{
			eATTACHMENT_POINT l_eAttach = l_attach->GetAttachmentPoint();

			if ( ( l_eAttach != eATTACHMENT_POINT_DEPTH && l_eAttach != eATTACHMENT_POINT_STENCIL ) || m_renderSystem->GetFeatureLevel() > D3D_FEATURE_LEVEL_10_1 )
			{
				if ( ( l_bDepth && l_eAttach == eATTACHMENT_POINT_DEPTH ) || ( l_bStencil && l_eAttach == eATTACHMENT_POINT_STENCIL ) || ( l_bColour && l_eAttach != eATTACHMENT_POINT_DEPTH && l_eAttach != eATTACHMENT_POINT_STENCIL ) )
				{
					ID3D11View * l_pSrc = GetSurface( l_eAttach );
					ID3D11View * l_pDst = l_pBuffer->GetSurface( l_eAttach );

					if ( l_pSrc && l_pDst )
					{
						l_arrayPairs.push_back( std::make_pair( l_pSrc, l_pDst ) );
					}
				}
			}
		}

		HRESULT l_hr = S_OK;
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

		for ( auto l_itArray : l_arrayPairs )
		{
			try
			{
				ID3D11Resource * l_pDstSurface;
				ID3D11Resource * l_pSrcSurface;
				l_itArray.first->GetResource( &l_pDstSurface );
				l_itArray.second->GetResource( &l_pSrcSurface );

				if ( l_pDstSurface && l_pSrcSurface )
				{
					D3D11_BOX l_box = { 0 };
					l_box.front = 0;
					l_box.back = 1;
					l_box.left = p_rectDst.left();
					l_box.right = p_rectDst.right();
					l_box.top = p_rectDst.top();
					l_box.bottom = p_rectDst.bottom();
					l_pDeviceContext->CopySubresourceRegion( l_pDstSurface, 0, p_rectDst.left(), p_rectDst.top(), 0, l_pSrcSurface, 0, NULL );//&l_box );
				}

				SafeRelease( l_pSrcSurface );
				SafeRelease( l_pDstSurface );
			}
			catch ( ... )
			{
				Logger::LogError( cuT( "Error while blitting frame buffer" ) );
				l_hr = E_FAIL;
			}
		}

		return l_hr == S_OK;
	}
}
