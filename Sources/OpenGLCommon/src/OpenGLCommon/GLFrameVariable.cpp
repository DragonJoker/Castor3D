#include "PrecompiledHeader.h"

#include "OpenGLCommon/GLFrameVariable.h"

using namespace Castor3D;

void GLFrameVariableBase :: GetVariableLocation( const GLchar * p_pVarName)
{
	m_glIndex = glGetUniformLocation( * m_uiParentProgram, p_pVarName);
	CheckGLError( CU_T( "GLFrameVariableBase :: GetVariableLocation - glGetUniformLocation"));
}