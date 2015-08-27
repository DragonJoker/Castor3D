#include "Dx11SamplerRenderer.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx11Render
{
	DxSamplerRenderer::DxSamplerRenderer( DxRenderSystem * p_pRenderSystem )
		:	SamplerRenderer( p_pRenderSystem )
		,	m_pDxRenderSystem( p_pRenderSystem )
		,	m_pSamplerState( NULL )
	{
	}

	DxSamplerRenderer::~DxSamplerRenderer()
	{
	}

	bool DxSamplerRenderer::Initialise()
	{
		HRESULT l_hr = S_OK;

		if ( !m_pSamplerState )
		{
			ID3D11Device 	*		l_pDevice	= m_pDxRenderSystem->GetDevice();
			m_tex2dSampler.AddressU				= DirectX11::Get( m_target->GetWrappingMode( eTEXTURE_UVW_U ) );
			m_tex2dSampler.AddressV				= DirectX11::Get( m_target->GetWrappingMode( eTEXTURE_UVW_V ) );
			m_tex2dSampler.AddressW				= DirectX11::Get( m_target->GetWrappingMode( eTEXTURE_UVW_W ) );
			m_tex2dSampler.BorderColor[0]		= 0;
			m_tex2dSampler.BorderColor[1]		= 0;
			m_tex2dSampler.BorderColor[2]		= 0;
			m_tex2dSampler.BorderColor[3]		= 1;
			m_tex2dSampler.ComparisonFunc		= D3D11_COMPARISON_ALWAYS;
			m_tex2dSampler.MaxAnisotropy		= 0;
			m_tex2dSampler.MaxLOD				= 6;
			m_tex2dSampler.MinLOD				= 0;
			m_tex2dSampler.MipLODBias			= 2;
			eINTERPOLATION_MODE l_eMinMode		= m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN );
			eINTERPOLATION_MODE l_eMagMode		= m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG );
			eINTERPOLATION_MODE l_eMipMode		= m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP );

			if ( l_eMinMode == l_eMipMode && l_eMipMode == l_eMagMode )
			{
				if ( l_eMinMode == eINTERPOLATION_MODE_NEAREST )
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
				}
				else if ( l_eMinMode == eINTERPOLATION_MODE_LINEAR )
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				}
				else
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_ANISOTROPIC;
					m_tex2dSampler.MaxAnisotropy	= UINT( m_target->GetMaxAnisotropy() );
				}
			}
			else if ( l_eMinMode == eINTERPOLATION_MODE_NEAREST )
			{
				if ( l_eMagMode == eINTERPOLATION_MODE_NEAREST )
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
				}
				else if ( l_eMagMode == eINTERPOLATION_MODE_LINEAR )
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				}
				else
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_ANISOTROPIC;
					m_tex2dSampler.MaxAnisotropy	= UINT( m_target->GetMaxAnisotropy() );
				}
			}
			else if ( l_eMinMode == eINTERPOLATION_MODE_LINEAR )
			{
				if ( l_eMagMode == eINTERPOLATION_MODE_NEAREST )
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
				}
				else if ( l_eMagMode == eINTERPOLATION_MODE_LINEAR )
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				}
				else
				{
					m_tex2dSampler.Filter			= D3D11_FILTER_ANISOTROPIC;
					m_tex2dSampler.MaxAnisotropy	= UINT( m_target->GetMaxAnisotropy() );
				}
			}
			else
			{
				m_tex2dSampler.Filter				= D3D11_FILTER_ANISOTROPIC;
				m_tex2dSampler.MaxAnisotropy		= UINT( m_target->GetMaxAnisotropy() );
			}

			l_hr = l_pDevice->CreateSamplerState( &m_tex2dSampler, &m_pSamplerState );
			dxDebugName( m_pSamplerState, SamplerState );
		}

		return l_hr == S_OK;
	}

	void DxSamplerRenderer::Cleanup()
	{
		SafeRelease( m_pSamplerState );
	}

	bool DxSamplerRenderer::Bind( eTEXTURE_DIMENSION CU_PARAM_UNUSED( p_eDimension ), uint32_t p_uiIndex )
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pDxRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->PSSetSamplers( p_uiIndex, 1, &m_pSamplerState );
		l_pDeviceContext->Release();
		return true;
	}

	void DxSamplerRenderer::Unbind()
	{
	}
}

#pragma warning( pop)
