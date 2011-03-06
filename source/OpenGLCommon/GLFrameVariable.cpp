#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlFrameVariable.h"

using namespace Castor3D;

void GlFrameVariableBase :: GetVariableLocation( const GLchar * p_pVarName)
{
	CheckGlError( m_glIndex = glGetUniformLocation( * m_uiParentProgram, p_pVarName), CU_T( "GlFrameVariableBase :: GetVariableLocation - glGetUniformLocation"));
}
