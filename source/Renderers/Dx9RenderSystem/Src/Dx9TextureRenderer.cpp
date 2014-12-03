#include "Dx9TextureRenderer.hpp"
#include "Dx9RenderSystem.hpp"
#include <Pixel.hpp>

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx9Render
{
	DxTextureRenderer::DxTextureRenderer( DxRenderSystem * p_pRenderSystem )
		:	TextureRenderer( p_pRenderSystem )
	{
	}

	DxTextureRenderer::~DxTextureRenderer()
	{
	}

	bool DxTextureRenderer::Render()
	{
		DWORD l_dwSampler = m_target->GetIndex();
		IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();

		if ( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_NONE )
		{
			l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_COLOROP,   DirectX9::Get( m_target->GetRgbFunction() ) );
			l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_COLORARG1, DirectX9::Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_0 ) ) );
			l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_COLORARG2, DirectX9::Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_1 ) ) );
		}

		if ( m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_NONE )
		{
			l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_ALPHAOP,   DirectX9::Get( m_target->GetAlpFunction() ) );
			l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_ALPHAARG1, DirectX9::Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_0 ) ) );
			l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_ALPHAARG2, DirectX9::Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_1 ) ) );
		}

		if ( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
		{
			l_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, true );
			l_pDevice->SetRenderState( D3DRS_ALPHAFUNC, DirectX9::Get( m_target->GetAlphaFunc() ) );
			l_pDevice->SetRenderState( D3DRS_ALPHAREF, DWORD( m_target->GetAlphaValue() * 255 ) );
		}

		return true;
	}

	void DxTextureRenderer::EndRender()
	{
		IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();

		if ( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
		{
			l_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, false );
		}
	}
}

#pragma warning( pop)
