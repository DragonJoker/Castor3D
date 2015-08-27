#include "Dx9VertexShader.hpp"
#include "Dx9ShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	DxVertexShader::DxVertexShader( DxShaderProgram * p_pParent )
		:	DxShaderObject( p_pParent, eSHADER_TYPE_VERTEX )
		,	m_pVertexShader( NULL )
	{
	}

	DxVertexShader::~DxVertexShader()
	{
		if ( m_pVertexShader )
		{
			m_pVertexShader->Release();
			m_pVertexShader = NULL;
		}
	}

	void DxVertexShader::CreateProgram()
	{
	}

	bool DxVertexShader::Compile()
	{
		bool l_bReturn = DxShaderObject::Compile();

		if ( l_bReturn )
		{
			DoRetrieveShader( static_cast< DxShaderProgram * >( m_pParent )->GetShaderEffect() );
		}

		return l_bReturn;
	}

	void DxVertexShader::Bind()
	{
	}

	void DxVertexShader::Unbind()
	{
	}

	void DxVertexShader::DoRetrieveShader( ID3DXEffect * p_pEffect )
	{
		p_pEffect->GetVertexShader( "mainVx", &m_pVertexShader );
	}
}
