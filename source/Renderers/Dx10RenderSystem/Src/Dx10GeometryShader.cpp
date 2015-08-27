#include "Dx10GeometryShader.hpp"
#include "Dx10ShaderProgram.hpp"
#include "Dx10RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
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
		m_pRenderSystem->GetDevice()->GSSetShader( m_pGeometryShader );
	}

	void DxGeometryShader::Unbind()
	{
		m_pRenderSystem->GetDevice()->GSSetShader( NULL );
	}

	void DxGeometryShader::DoRetrieveShader()
	{
		if ( m_pCompiled )
		{
			ID3D10Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateGeometryShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), &m_pGeometryShader );
				dxDebugName( m_pGeometryShader, GSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}
}
