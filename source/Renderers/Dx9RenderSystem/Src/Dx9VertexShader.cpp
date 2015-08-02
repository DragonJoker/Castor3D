#include "Dx9VertexShader.hpp"
#include "Dx9ShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	static xchar const * const WARNING_VERTEX_SHADER = cuT( "Couldn't retrieve a VertexShader from the D3DXEffect: 0x%x (%s)" );

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

	void DxVertexShader::Bind()
	{
	}

	void DxVertexShader::Unbind()
	{
	}
}
