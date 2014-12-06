#include "Dx10VertexShader.hpp"
#include "Dx10ShaderProgram.hpp"
#include "Dx10RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;
namespace Dx10Render
{
	DxVertexShader::DxVertexShader( DxShaderProgram * p_pParent )
		:	DxShaderObject( p_pParent, eSHADER_TYPE_VERTEX )
		,	m_pVertexShader( NULL )
	{
	}

	DxVertexShader::~DxVertexShader()
	{
		SafeRelease( m_pVertexShader );
	}

	void DxVertexShader::Bind()
	{
		m_pRenderSystem->GetDevice()->VSSetShader( m_pVertexShader );
	}

	void DxVertexShader::Unbind()
	{
		m_pRenderSystem->GetDevice()->VSSetShader( NULL );
	}

	void DxVertexShader::DoRetrieveShader()
	{
		if ( m_pCompiled )
		{
			ID3D10Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateVertexShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), &m_pVertexShader );
				dxDebugName( m_pVertexShader, VSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}
}
