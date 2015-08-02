#include "Dx9SamplerRenderer.hpp"
#include "Dx9RenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx9Render
{
	Castor3D::SamplerSPtr DxSamplerRenderer::sm_pCurrent = nullptr;

	DxSamplerRenderer::DxSamplerRenderer( DxRenderSystem * p_pRenderSystem )
		:	SamplerRenderer( p_pRenderSystem )
		,	m_pDxRenderSystem( p_pRenderSystem )
	{
	}

	DxSamplerRenderer::~DxSamplerRenderer()
	{
	}

	bool DxSamplerRenderer::Initialise()
	{
		if ( !sm_pCurrent )
		{
			sm_pCurrent = std::make_shared< Sampler >( m_pRenderSystem->GetEngine(), cuT( "Dx9DefaultSampler" ) );
			sm_pCurrent->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_REPEAT );
			sm_pCurrent->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_REPEAT );
			sm_pCurrent->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_NEAREST );
			sm_pCurrent->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_NEAREST );
			sm_pCurrent->SetBorderColour( Colour::from_bgra( 0 ) );
		}

		return true;
	}

	void DxSamplerRenderer::Cleanup()
	{
	}

	bool DxSamplerRenderer::Bind( eTEXTURE_DIMENSION CU_PARAM_UNUSED( p_eDimension ), uint32_t p_uiIndex )
	{
		IDirect3DDevice9 *	l_pDevice		= m_pDxRenderSystem->GetDevice();
		D3DCOLOR l_d3dColour = m_target->GetBorderColour().to_argb();

		if ( sm_pCurrent->GetWrappingMode( eTEXTURE_UVW_U ) != m_target->GetWrappingMode( eTEXTURE_UVW_U ) )
		{
			sm_pCurrent->SetWrappingMode( eTEXTURE_UVW_U, m_target->GetWrappingMode( eTEXTURE_UVW_U ) );
			l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_ADDRESSU, DirectX9::Get( m_target->GetWrappingMode( eTEXTURE_UVW_U ) ) );
		}

		if ( sm_pCurrent->GetWrappingMode( eTEXTURE_UVW_V ) != m_target->GetWrappingMode( eTEXTURE_UVW_V ) )
		{
			sm_pCurrent->SetWrappingMode( eTEXTURE_UVW_V, m_target->GetWrappingMode( eTEXTURE_UVW_V ) );
			l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_ADDRESSV, DirectX9::Get( m_target->GetWrappingMode( eTEXTURE_UVW_V ) ) );
		}

		if ( sm_pCurrent->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) != m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) )
		{
			sm_pCurrent->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) );
			l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_MINFILTER, DirectX9::Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) ) );
		}

		if ( sm_pCurrent->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) != m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) )
		{
			sm_pCurrent->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) );
			l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_MAGFILTER, DirectX9::Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) ) );
		}

		if ( sm_pCurrent->GetBorderColour() != m_target->GetBorderColour() )
		{
			sm_pCurrent->SetBorderColour( m_target->GetBorderColour() );
			l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_BORDERCOLOR, l_d3dColour );
		}

		return true;
	}

	void DxSamplerRenderer::Unbind()
	{
	}
}

#pragma warning( pop)
