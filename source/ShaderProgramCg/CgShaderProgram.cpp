#include "ShaderProgramCg/PrecompiledHeader.hpp"

#include "ShaderProgramCg/CgFrameVariable.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

//*************************************************************************************************

CgShaderProgram :: CgShaderProgram( RenderSystem * p_pRenderSystem )
	:	ShaderProgramBase( p_pRenderSystem, eSHADER_LANGUAGE_CG )
{
}

CgShaderProgram :: ~CgShaderProgram()
{
}

String CgShaderProgram :: DoGetVertexShaderSource( uint32_t p_uiProgramFlags )
{
	String	l_strReturn;

	return l_strReturn;
}

String CgShaderProgram :: DoGetPixelShaderSource( uint32_t p_uiFlags )
{
	String	l_strReturn;

	return l_strReturn;
}

String CgShaderProgram :: DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass )
{
	String	l_strReturn;

	return l_strReturn;
}

String CgShaderProgram :: DoGetDeferredPixelShaderSource( uint32_t p_uiFlags )
{
	String	l_strReturn;

	return l_strReturn;
}

//*************************************************************************************************
