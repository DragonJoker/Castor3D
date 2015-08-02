#include "Dx9FragmentShader.hpp"
#include "Dx9ShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	static xchar const * const WARNING_PIXEL_SHADER = cuT( "Couldn't retrieve a PixelShader from the D3DXEffect: 0x%x (%s)" );

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

	void DxFragmentShader::Bind()
	{
	}

	void DxFragmentShader::Unbind()
	{
	}
}
