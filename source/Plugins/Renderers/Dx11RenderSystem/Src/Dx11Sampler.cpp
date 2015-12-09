#include "Dx11Sampler.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	DxSampler::DxSampler( DxRenderSystem * p_renderSystem, Castor::String const & p_name )
		: Sampler( *p_renderSystem->GetOwner(), p_name )
		, m_pDxRenderSystem( p_renderSystem )
		, m_pSamplerState( NULL )
	{
	}

	DxSampler::~DxSampler()
	{
	}

	bool DxSampler::Initialise()
	{
		HRESULT l_hr = S_OK;

		if ( !m_pSamplerState )
		{
			ID3D11Device * l_pDevice = m_pDxRenderSystem->GetDevice();
			m_tex2dSampler.AddressU = DirectX11::Get( GetWrappingMode( eTEXTURE_UVW_U ) );
			m_tex2dSampler.AddressV = DirectX11::Get( GetWrappingMode( eTEXTURE_UVW_V ) );
			m_tex2dSampler.AddressW = DirectX11::Get( GetWrappingMode( eTEXTURE_UVW_W ) );
			m_tex2dSampler.BorderColor[0] = 0;
			m_tex2dSampler.BorderColor[1] = 0;
			m_tex2dSampler.BorderColor[2] = 0;
			m_tex2dSampler.BorderColor[3] = 1;
			m_tex2dSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			m_tex2dSampler.MaxAnisotropy = 0;
			m_tex2dSampler.MaxLOD = 6;
			m_tex2dSampler.MinLOD = 0;
			m_tex2dSampler.MipLODBias = 2;
			eINTERPOLATION_MODE l_eMinMode = GetInterpolationMode( eINTERPOLATION_FILTER_MIN );
			eINTERPOLATION_MODE l_eMagMode = GetInterpolationMode( eINTERPOLATION_FILTER_MAG );
			eINTERPOLATION_MODE l_eMipMode = GetInterpolationMode( eINTERPOLATION_FILTER_MIP );

			if ( l_eMinMode == l_eMipMode && l_eMipMode == l_eMagMode )
			{
				if ( l_eMinMode == eINTERPOLATION_MODE_NEAREST )
				{
					m_tex2dSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				}
				else if ( l_eMinMode == eINTERPOLATION_MODE_LINEAR )
				{
					m_tex2dSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				}
				else
				{
					m_tex2dSampler.Filter = D3D11_FILTER_ANISOTROPIC;
					m_tex2dSampler.MaxAnisotropy = UINT( GetMaxAnisotropy() );
				}
			}
			else if ( l_eMinMode == eINTERPOLATION_MODE_NEAREST )
			{
				if ( l_eMagMode == eINTERPOLATION_MODE_NEAREST )
				{
					m_tex2dSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				}
				else if ( l_eMagMode == eINTERPOLATION_MODE_LINEAR )
				{
					m_tex2dSampler.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				}
				else
				{
					m_tex2dSampler.Filter = D3D11_FILTER_ANISOTROPIC;
					m_tex2dSampler.MaxAnisotropy = UINT( GetMaxAnisotropy() );
				}
			}
			else if ( l_eMinMode == eINTERPOLATION_MODE_LINEAR )
			{
				if ( l_eMagMode == eINTERPOLATION_MODE_NEAREST )
				{
					m_tex2dSampler.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
				}
				else if ( l_eMagMode == eINTERPOLATION_MODE_LINEAR )
				{
					m_tex2dSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				}
				else
				{
					m_tex2dSampler.Filter = D3D11_FILTER_ANISOTROPIC;
					m_tex2dSampler.MaxAnisotropy = UINT( GetMaxAnisotropy() );
				}
			}
			else
			{
				m_tex2dSampler.Filter = D3D11_FILTER_ANISOTROPIC;
				m_tex2dSampler.MaxAnisotropy = UINT( GetMaxAnisotropy() );
			}

			l_hr = l_pDevice->CreateSamplerState( &m_tex2dSampler, &m_pSamplerState );
			dxTrack( static_cast< DxRenderSystem * >( GetOwner()->GetRenderSystem() ), m_pSamplerState, SamplerState );
		}

		return l_hr == S_OK;
	}

	void DxSampler::Cleanup()
	{
		ReleaseTracked( GetOwner()->GetRenderSystem(), m_pSamplerState );
	}

	bool DxSampler::Bind( eTEXTURE_TYPE CU_PARAM_UNUSED( p_dimension ), uint32_t p_index )
	{
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( GetOwner()->GetRenderSystem()->GetCurrentContext() )->GetDeviceContext();
		l_deviceContext->PSSetSamplers( p_index, 1, &m_pSamplerState );
		return true;
	}

	void DxSampler::Unbind()
	{
	}
}
