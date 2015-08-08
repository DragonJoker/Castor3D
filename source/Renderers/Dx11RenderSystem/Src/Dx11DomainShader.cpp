#include "Dx11DomainShader.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxDomainShader::DxDomainShader( DxShaderProgram * p_pParent )
		: DxShaderObject( p_pParent, eSHADER_TYPE_DOMAIN )
		, m_pDomainShader( NULL )
	{
	}

	DxDomainShader::~DxDomainShader()
	{
		SafeRelease( m_pDomainShader );
	}

	void DxDomainShader::DoBind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->DSSetShader( m_pDomainShader, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxDomainShader::DoUnbind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->DSSetShader( NULL, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxDomainShader::DoRetrieveShader()
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateDomainShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pDomainShader );
				dxDebugName( m_pDomainShader, DSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}
}
