#include "Dx9FragmentShader.hpp"
#include "Dx9ShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	DxFragmentShader::DxFragmentShader( DxShaderProgram * p_pParent )
		:	DxShaderObject( p_pParent, eSHADER_TYPE_PIXEL )
		,	m_pPixelShader( NULL )
	{
	}

	DxFragmentShader::~DxFragmentShader()
	{
		if ( m_pPixelShader )
		{
			m_pPixelShader->Release();
			m_pPixelShader = NULL;
		}
	}

	void DxFragmentShader::CreateProgram()
	{
	}

	bool DxFragmentShader::Compile()
	{
		bool l_bReturn = DxShaderObject::Compile();

		if ( l_bReturn )
		{
			DoRetrieveShader( static_cast< DxShaderProgram * >( m_pParent )->GetShaderEffect() );
		}

		return l_bReturn;
	}

	void DxFragmentShader::Bind()
	{
	}

	void DxFragmentShader::Unbind()
	{
	}

	void DxFragmentShader::DoRetrieveShader( ID3DXEffect * p_pEffect )
	{
		p_pEffect->GetPixelShader( "mainPx", &m_pPixelShader );
	}
}
