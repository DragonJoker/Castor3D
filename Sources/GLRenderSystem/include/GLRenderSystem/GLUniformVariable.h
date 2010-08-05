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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
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

	class GLOneIntUniformVariable : public OneUniformVariable<int>, public GLUniformVariable
	{
	public:
		GLOneIntUniformVariable( OneUniformVariable<int> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				OneUniformVariable<int>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLTwoIntsUniformVariable : public Point2DUniformVariable<int>, public GLUniformVariable
	{
	public:
		GLTwoIntsUniformVariable( Point2DUniformVariable<int> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Point2DUniformVariable<int>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLThreeIntsUniformVariable : public Point3DUniformVariable<int>, public GLUniformVariable
	{
	public:
		GLThreeIntsUniformVariable( Point3DUniformVariable<int> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Point3DUniformVariable<int>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLFourIntsUniformVariable : public Point4DUniformVariable<int>, public GLUniformVariable
	{
	public:
		GLFourIntsUniformVariable( Point4DUniformVariable<int> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Point4DUniformVariable<int>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLOneFloatUniformVariable : public OneUniformVariable<float>, public GLUniformVariable
	{
	public:
		GLOneFloatUniformVariable( OneUniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				OneUniformVariable<float>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLTwoFloatsUniformVariable : public Point2DUniformVariable<float>, public GLUniformVariable
	{
	public:
		GLTwoFloatsUniformVariable( Point2DUniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Point2DUniformVariable<float>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLThreeFloatsUniformVariable : public Point3DUniformVariable<float>, public GLUniformVariable
	{
	public:
		GLThreeFloatsUniformVariable( Point3DUniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Point3DUniformVariable<float>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLFourFloatsUniformVariable : public Point4DUniformVariable<float>, public GLUniformVariable
	{
	public:
		GLFourFloatsUniformVariable( Point4DUniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Point4DUniformVariable<float>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLMatrix2fUniformVariable : public Matrix2UniformVariable<float>, public GLUniformVariable
	{
	public:
		GLMatrix2fUniformVariable( Matrix2UniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Matrix2UniformVariable<float>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLMatrix3fUniformVariable : public Matrix3UniformVariable<float>, public GLUniformVariable
	{
	public:
		GLMatrix3fUniformVariable( Matrix3UniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Matrix3UniformVariable<float>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};

	class GLMatrix4fUniformVariable : public Matrix4UniformVariable<float>, public GLUniformVariable
	{
	public:
		GLMatrix4fUniformVariable( Matrix4UniformVariable<float> * p_pUniformVariable, GLuint * p_uiProgram)
			:	GLUniformVariable( p_uiProgram),
				Matrix4UniformVariable<float>( *p_pUniformVariable)
		{}
		virtual void Apply();
	};
}

#endif