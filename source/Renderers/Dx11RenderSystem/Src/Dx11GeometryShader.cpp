#include "Dx11GeometryShader.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxGeometryShader::DxGeometryShader( DxShaderProgram * p_pParent )
		:	DxShaderObject( p_pParent, eSHADER_TYPE_GEOMETRY )
		,	m_pGeometryShader( NULL )
	{
	}

	DxGeometryShader::~DxGeometryShader()
	{
		SafeRelease( m_pGeometryShader );
	}

	void DxGeometryShader::Bind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->GSSetShader( m_pGeometryShader, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxGeometryShader::Unbind()
	{
		ID3D11DeviceContext * l_pDeviceContext;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
		l_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		l_pDeviceContext->Release();
	}

	void DxGeometryShader::DoRetrieveShader()
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateGeometryShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pGeometryShader );
				dxDebugName( m_pGeometryShader, GSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}
}
