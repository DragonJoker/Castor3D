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

void GLOneIntUniformVariable :: Apply()
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
}

void GLTwoIntsUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform2iv( m_glIndex, 2, m_ptValue.ptr());
}

void GLThreeIntsUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform3iv( m_glIndex, 3, m_ptValue.ptr());
}

void GLFourIntsUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform4iv( m_glIndex, 4, m_ptValue.ptr());
}

void GLOneFloatUniformVariable :: Apply()
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
}

void GLTwoFloatsUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform2fv( m_glIndex, 2, m_ptValue.ptr());
}

void GLThreeFloatsUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform3fv( m_glIndex, 3, m_ptValue.ptr());
}

void GLFourFloatsUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniform4fv( m_glIndex, 4, m_ptValue.ptr());
}

void GLMatrix2fUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniformMatrix2fv( m_glIndex, 4, false, (GLfloat *)m_mValue.m_matrix);
}

void GLMatrix3fUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniformMatrix3fv( m_glIndex, 9, false, (GLfloat *)m_mValue.m_matrix);
}

void GLMatrix4fUniformVariable :: Apply()
{
	if (m_glIndex == -1)
	{
		GetUniformLocation( m_strName.char_str());
	}

	if (m_glIndex == -1) 
	{
		return;
	}

	glUniformMatrix4fv( m_glIndex, 16, false, (GLfloat *)m_mValue.m_matrix);
}