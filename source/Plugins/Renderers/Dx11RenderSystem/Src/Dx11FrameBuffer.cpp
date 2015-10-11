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

	bool DxFrameBuffer::SetDrawBuffers( BufAttachArray const & p_attaches )
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
						l_pView = reinterpret_cast< ID3D11DepthStencilView * >( std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_rboAttach->GetRenderBuffer() )->GetDxRenderBuffer().GetSurface() );
					}
					else if ( l_eAttach == eATTACHMENT_POINT_COLOUR )
					{
						l_arraySurfaces.push_back( reinterpret_cast< ID3D11RenderTargetView * >( std::static_pointer_cast< DxColourRenderBuffer >( l_rboAttach->GetRenderBuffer() )->GetDxRenderBuffer().GetSurface() ) );
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

	ID3D11View * DxFrameBuffer::GetSurface( Castor3D::eATTACHMENT_POINT p_eAttach )
	{
		ID3D11View * l_pReturn = NULL;

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
					l_pReturn = std::static_pointer_cast< DxDynamicTexture >( l_attach->GetTexture() )->GetRenderTargetView();
				}
				else
				{
					RenderBufferAttachmentSPtr l_attach = std::static_pointer_cast< RenderBufferAttachment >( *l_it );
					eATTACHMENT_POINT l_eAttach = l_attach->GetAttachmentPoint();

					if ( l_eAttach == eATTACHMENT_POINT_DEPTH || l_eAttach == eATTACHMENT_POINT_STENCIL )
					{
						l_pReturn = std::static_pointer_cast< DxDepthStencilRenderBuffer >( l_attach->GetRenderBuffer() )->GetDxRenderBuffer().GetSurface();
					}
					else
					{
						l_pReturn = std::static_pointer_cast< DxColourRenderBuffer >( l_attach->GetRenderBuffer() )->GetDxRenderBuffer().GetSurface();
					}
				}
			}
		}

		return l_pReturn;
	}

	bool DxFrameBuffer::DoBind( eFRAMEBUFFER_TARGET p_eTarget )
	{
		return true;
	}

	void DxFrameBuffer::DoUnbind()
	{
#if DX_DEBUG_RT

		if ( !m_attaches.empty() )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

			for ( auto && l_attach : m_attaches )
			{
				eATTACHMENT_POINT l_eAttach = l_attach->GetAttachmentPoint();

				if ( l_eAttach == eATTACHMENT_POINT_COLOUR )
				{
					ID3D11Resource * l_pResource = NULL;
					TextureAttachmentSPtr l_texAttach = std::static_pointer_cast< TextureAttachment >( l_attach );
					std::static_pointer_cast< DxDynamicTexture >( l_texAttach->GetTexture() )->GetRenderTargetView()->GetResource( &l_pResource );
					StringStream l_name;
					l_name << Engine::GetEngineDirectory() << cuT( "\\DynamicTexture_" ) << ( void * )l_texAttach->GetTexture().get() << cuT( "_FBA.png" );
					D3DX11SaveTextureToFile( l_pDeviceContext, l_pResource, D3DX11_IFF_PNG, l_name.str().c_str() );
					l_pResource->Release();
				}
			}
		}

#endif
	}

	bool DxFrameBuffer::DoAttach( eATTACHMENT_POINT CU_PARAM_UNUSED( p_attachment ), uint8_t CU_PARAM_UNUSED( p_index ), TextureAttachmentSPtr CU_PARAM_UNUSED( p_texture ), eTEXTURE_TARGET CU_PARAM_UNUSED( p_target ), int CU_PARAM_UNUSED( p_layer ) )
	{
		return true;
	}

	bool DxFrameBuffer::DoAttach( eATTACHMENT_POINT CU_PARAM_UNUSED( p_attachment ), uint8_t CU_PARAM_UNUSED( p_index ), RenderBufferAttachmentSPtr CU_PARAM_UNUSED( p_renderBuffer ) )
	{
		return true;
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
