#include "Dx9RasteriserState.hpp"
#include "Dx9RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	Castor3D::RasteriserStateSPtr DxRasteriserState::sm_pCurrent;

	DxRasteriserState::DxRasteriserState( DxRenderSystem * p_pRenderSystem )
		:	RasteriserState(	)
		,	m_pRenderSystem( p_pRenderSystem	)
	{
	}

	DxRasteriserState::~DxRasteriserState()
	{
	}

	bool DxRasteriserState::Initialise()
	{
		if ( !sm_pCurrent )
		{
			sm_pCurrent = std::make_shared< DxRasteriserState >( m_pRenderSystem );
		}

		m_bChanged = false;
		return true;
	}

	void DxRasteriserState::Cleanup()
	{
	}

	bool DxRasteriserState::Apply()
	{
		bool l_bReturn = true;

		if ( sm_pCurrent->GetAntialiasedLines() != GetAntialiasedLines() )
		{
			sm_pCurrent->SetAntialiasedLines( GetAntialiasedLines() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, GetAntialiasedLines() ) == S_OK;
		}

		if ( sm_pCurrent->GetDepthBias() != GetDepthBias() )
		{
			sm_pCurrent->SetDepthBias( GetDepthBias() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DEPTHBIAS, 0 ) == S_OK;
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, DWORD( GetDepthBias() ) ) == S_OK;
		}

		if ( sm_pCurrent->GetCulledFaces() != GetCulledFaces() )
		{
			sm_pCurrent->SetCulledFaces( GetCulledFaces() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_CULLMODE, DirectX9::Get( GetCulledFaces() ) ) == S_OK;
		}

		if ( sm_pCurrent->GetFillMode() != GetFillMode() )
		{
			sm_pCurrent->SetFillMode( GetFillMode() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_FILLMODE, DirectX9::Get( GetFillMode() ) ) == S_OK;
		}

		if ( sm_pCurrent->GetMultisample() != GetMultisample() )
		{
			sm_pCurrent->SetMultisample( GetMultisample() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, GetMultisample() ) == S_OK;
		}

		if ( sm_pCurrent->GetDepthClipping() != GetDepthClipping() )
		{
			sm_pCurrent->SetDepthClipping( GetDepthClipping() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_CLIPPLANEENABLE, GetDepthClipping() ) == S_OK;
		}

		if ( sm_pCurrent->GetScissor() != GetScissor() )
		{
			sm_pCurrent->SetScissor( GetScissor() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, GetScissor() ) == S_OK;
		}

		return l_bReturn;
	}
}
