#include "Dx11VertexShader.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxVertexShader::DxVertexShader( DxShaderProgram * p_pParent )
		: DxShaderObject( p_pParent, eSHADER_TYPE_VERTEX )
		, m_pVertexShader( NULL )
	{
	}

	DxVertexShader::~DxVertexShader()
	{
		SafeRelease( m_pVertexShader );
	}

	void DxVertexShader::Bind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxVertexShader::Unbind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->VSSetShader( NULL, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxVertexShader::DoRetrieveShader()
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateVertexShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pVertexShader );
				dxDebugName( m_pVertexShader, VSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}
}
