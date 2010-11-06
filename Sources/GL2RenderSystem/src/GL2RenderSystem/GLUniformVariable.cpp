#include "PrecompiledHeader.h"

#include "GLUniformVariable.h"
#include "GLRenderSystem.h"

using namespace Castor3D;

void GLUniformVariable :: GetUniformLocation( const GLchar * p_pVarName)
{
	m_glIndex = glGetUniformLocation( * m_uiParentProgram, p_pVarName);
	CheckGLError( "GLShaderProgram :: GetUniformLocation - glGetUniformLocation");
}

void GLIntUniformVariable :: Apply()
{
	if (m_glIndex == GL_INVALID_INDEX)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == GL_INVALID_INDEX) 
	{
		return;
	}

	glUniform1i( m_glIndex, m_tValue);
}

void GLFloatUniformVariable :: Apply()
{
	if (m_glIndex == GL_INVALID_INDEX)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == GL_INVALID_INDEX) 
	{
		return;
	}

	glUniform1f( m_glIndex, m_tValue);
}