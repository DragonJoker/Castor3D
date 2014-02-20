#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_OpenGL )
#include "ShaderProgramCg/CgGlShaderProgram.hpp"
#include "ShaderProgramCg/CgGlShaderObject.hpp"
#include "ShaderProgramCg/CgGlFrameVariable.hpp"

#include <GlRenderSystem/GlRenderSystem.hpp>

using namespace CgShader;
using namespace Castor3D;
using namespace Castor;
using namespace GlRender;

CgGlShaderProgram :: CgGlShaderProgram( RenderSystem * p_pRenderSystem )
	:	CgShaderProgram( p_pRenderSystem )
{
}

CgGlShaderProgram :: ~CgGlShaderProgram()
{
	ShaderProgramBase::Cleanup();
}

void CgGlShaderProgram :: Begin( uint8_t p_byIndex, uint8_t p_byCount )
{
	m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
	ShaderProgramBase::Begin( p_byIndex, p_byCount );
}

int CgGlShaderProgram :: GetAttributeLocation( String const & p_strName)const
{
	int l_iReturn = eGL_INVALID_INDEX;
	CgShaderObjectPtr l_pShader = std::static_pointer_cast< CgShaderObject >( m_pShaders[eSHADER_TYPE_VERTEX] );

	if (l_pShader)
	{
		CGprogram l_program = l_pShader->GetProgram();

		if (l_program)
		{
			uint32_t l_uiProgram = cgGLGetProgramID( l_program);

			if (l_uiProgram != eGL_INVALID_INDEX && l_uiProgram != 0)
			{
//				l_iReturn = OpenGl::GetAttribLocation( l_uiProgram, str_utils::to_str( p_strName ).c_str());
			}
		}
	}

	return l_iReturn;
}

ShaderObjectBaseSPtr CgGlShaderProgram :: DoCreateObject(eSHADER_TYPE p_eType)
{
	ShaderObjectBaseSPtr l_pReturn( new CgGlShaderObject( this, p_eType));
	return l_pReturn;
}

std::shared_ptr< OneTextureFrameVariable > CgGlShaderProgram :: DoCreateTextureVariable( int p_iNbOcc )
{
	std::shared_ptr< OneTextureFrameVariable > l_pReturn( new CgGlOneFrameVariable< TextureBaseRPtr >( this, p_iNbOcc ) );
	return l_pReturn;
}
#endif