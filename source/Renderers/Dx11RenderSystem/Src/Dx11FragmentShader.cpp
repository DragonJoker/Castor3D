#include "Dx11FragmentShader.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxFragmentShader::DxFragmentShader( DxShaderProgram * p_pParent )
		:	DxShaderObject( p_pParent, eSHADER_TYPE_PIXEL )
		,	m_pPixelShader( NULL )
	{
	}

	DxFragmentShader::~DxFragmentShader()
	{
		SafeRelease( m_pPixelShader );
	}

	void DxFragmentShader::Bind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxFragmentShader::Unbind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->PSSetShader( NULL, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxFragmentShader::DoRetrieveShader()
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreatePixelShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pPixelShader );
				dxDebugName( m_pPixelShader, PSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}
}
