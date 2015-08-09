#include "Dx11HullShader.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxHullShader::DxHullShader( DxShaderProgram * p_pParent )
		: DxShaderObject( p_pParent, eSHADER_TYPE_HULL )
		, m_pHullShader( NULL )
	{
	}

	DxHullShader::~DxHullShader()
	{
		SafeRelease( m_pHullShader );
	}

	void DxHullShader::DoBind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->HSSetShader( m_pHullShader, NULL, 0 );
	}

	void DxHullShader::DoUnbind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->HSSetShader( NULL, NULL, 0 );
	}

	void DxHullShader::DoRetrieveShader()
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateHullShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pHullShader );
				dxDebugName( m_pHullShader, HSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}
}
