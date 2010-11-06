#include "PrecompiledHeader.h"

#include "GLUniformVariable.h"
#include "GLRenderSystem.h"

using namespace Castor3D;

void GLUniformVariable :: GetUniformLocation( const GLchar * p_pVarName)
{
	m_glIndex = glGetUniformLocation( * m_uiParentProgram, p_pVarName);
	CheckGLError( "GLShaderProgram :: GetUniformLocation - glGetUniformLocation");

	if (m_glIndex == -1) 
	{
		Log::LogMessage( "Error: can't find uniform variable \"%s\"", p_pVarName);
	}
}

void GLIntUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform1i( m_glIndex, m_tValue);
	CheckGLError( "GLOneIntUniformVariable :: Apply - glUniform1i");
}

void GLFloatUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform1f( m_glIndex, m_tValue);
	CheckGLError( "GLOneFloatUniformVariable :: Apply - glUniform1f");
}