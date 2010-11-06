/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_UniformVariable___
#define ___GL_UniformVariable___

#include "Module_GLSL.h"
#include "Module_GL.h"

namespace Castor3D
{
	class GLUniformVariable
	{
	protected:
		int m_glIndex;
		GLuint * m_uiParentProgram;

	public:
		GLUniformVariable( GLuint * p_uiProgram)
			:	m_glIndex( -1),
				m_uiParentProgram( p_uiProgram)
		{
		}
		virtual void Apply()=0;

	protected:
		void GetUniformLocation( const GLchar * p_pVarName);
	};

	class GLIntUniformVariable : public OneUniformVariable<int>, public GLUniformVariable
	{
	public:
		GLIntUniformVariable( OneUniformVariable<int> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				OneUniformVariable<int>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	template <size_t Count>
	class GLIntsUniformVariable : public PointUniformVariable<int, Count>, public GLUniformVariable
	{
	public:
		GLIntsUniformVariable( PointUniformVariable<int, Count> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				PointUniformVariable<int, Count>( * p_pUniformVariable)
		{}
		virtual void Apply()
		{
			if (m_glIndex == GL_INVALID_INDEX)
			{
				GetUniformLocation( m_strName.char_str());
			}

			if (m_glIndex == GL_INVALID_INDEX) 
			{
				return;
			}

			switch (Count)
			{
			case 2:
				glUniform2iv( m_glIndex, 1, m_ptValue.m_coords);
				break;

			case 3:
				glUniform3iv( m_glIndex, 1, m_ptValue.m_coords);
				break;

			case 4:
				glUniform4iv( m_glIndex, 1, m_ptValue.m_coords);
				break;
			}

			GLRenderSystem::GL_CheckError( "GLIntsUniformVariable :: Apply - glUniform<Count>iv");
		}
	};

	class GLFloatUniformVariable : public OneUniformVariable<float>, public GLUniformVariable
	{
	public:
		GLFloatUniformVariable( OneUniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				OneUniformVariable<float>( * p_pUniformVariable)
		{}
		virtual void Apply();
	};

	template <size_t Count>
	class GLFloatsUniformVariable : public PointUniformVariable<float, Count>, public GLUniformVariable
	{
	public:
		GLFloatsUniformVariable( PointUniformVariable<float, Count> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				PointUniformVariable<float, Count>( * p_pUniformVariable)
		{}
		virtual void Apply()
		{
			if (m_glIndex == GL_INVALID_INDEX)
			{
				GetUniformLocation( m_strName.char_str());
			}

			if (m_glIndex == GL_INVALID_INDEX) 
			{
				return;
			}

			switch (Count)
			{
			case 2:
				glUniform2fv( m_glIndex, 1, m_ptValue.m_coords);
				break;

			case 3:
				glUniform3fv( m_glIndex, 1, m_ptValue.m_coords);
				break;

			case 4:
				glUniform4fv( m_glIndex, 1, m_ptValue.m_coords);
				break;
			}

			GLRenderSystem::GL_CheckError( "GLFloatsUniformVariable :: Apply - glUniform<Count>fv");
		}
	};

	template <size_t Count>
	class GLMatrixUniformVariable : public MatrixUniformVariable<float, Count>, public GLUniformVariable
	{
	public:
		GLMatrixUniformVariable( MatrixUniformVariable<float, Count> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				MatrixUniformVariable<float, Count>( * p_pUniformVariable)
		{}
		virtual void Apply()
		{
			if (m_glIndex == GL_INVALID_INDEX)
			{
				GetUniformLocation( m_strName.char_str());
			}

			if (m_glIndex == GL_INVALID_INDEX) 
			{
				return;
			}

			switch (Count)
			{
			case 2:
				glUniformMatrix2fv( m_glIndex, 1, false, (GLfloat *)m_mValue.m_matrix);
				break;

			case 3:
				glUniformMatrix3fv( m_glIndex, 1, false, (GLfloat *)m_mValue.m_matrix);
				break;

			case 4:
				glUniformMatrix4fv( m_glIndex, 1, false, (GLfloat *)m_mValue.m_matrix);
				break;
			}

			GLRenderSystem::GL_CheckError( "GLMatrixUniformVariable :: Apply - glUniformMatrix3fv");
		}
	};
}

#endif