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
#ifndef ___GL_FrameVariable___
#define ___GL_FrameVariable___

#include "Module_GLRender.h"

namespace Castor3D
{
	template <typename Type> inline void ApplyVariable( int p_iGLIndex, const Type * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Count> inline void ApplyVariable( int p_iGLIndex, const Type * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Rows, size_t Columns> inline void ApplyVariable( int p_iGLIndex, const Type * p_pValue, size_t p_uiOcc);

	class GLFrameVariableBase
	{
	protected:
		int m_glIndex;
		GLuint * m_uiParentProgram;
		bool m_bPresentInProgram;

	public:
		GLFrameVariableBase( GLuint * p_uiProgram)
			:	m_glIndex( -1),
				m_uiParentProgram( p_uiProgram),
				m_bPresentInProgram( true)
		{}
		virtual ~GLFrameVariableBase()
		{}
		virtual void Apply()=0;

	protected:
		void GetVariableLocation( const GLchar * p_pVarName);
		template <typename Type> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
		template <typename Type, int Count> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
		template <typename Type, int Rows, int Columns> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
	};

	template <typename T>
	class GLOneFrameVariable : public OneFrameVariable<T>, public GLFrameVariableBase, public MemoryTraced< GLOneFrameVariable<T> >
	{
	public:
		GLOneFrameVariable( OneFrameVariable<T> * p_pVariable, GLuint * p_uiProgram)
			:	GLFrameVariableBase( p_uiProgram),
				OneFrameVariable<T>( *p_pVariable)
		{
		}
		virtual ~GLOneFrameVariable()
		{
		}
		virtual void Apply()
		{
			if (m_bChanged)
			{
				_apply<T>( m_strName, m_tValue, m_uiOcc);
				m_bChanged = false;
			}
		}
	};

	template <typename T, size_t Count>
	class GLPointFrameVariable : public PointFrameVariable<T, Count>, public GLFrameVariableBase, public MemoryTraced< GLPointFrameVariable<T, Count> >
	{
	public:
		GLPointFrameVariable( PointFrameVariable<T, Count> * p_pVariable, GLuint * p_uiProgram)
			:	GLFrameVariableBase( p_uiProgram),
				PointFrameVariable<T, Count>( * p_pVariable)
		{
		}
		virtual ~GLPointFrameVariable()
		{
		}
		virtual void Apply()
		{
			if (m_bChanged)
			{
				_apply<T, Count>( m_strName, m_pValues, m_uiOcc);
				m_bChanged = false;
			}
		}
	};

	template <typename T, size_t Rows, size_t Columns>
	class GLMatrixFrameVariable : public MatrixFrameVariable<T, Rows, Columns>, public GLFrameVariableBase, public MemoryTraced< GLMatrixFrameVariable<T, Rows, Columns> >
	{
	public:
		GLMatrixFrameVariable( MatrixFrameVariable<T, Rows, Columns> * p_pVariable, GLuint * p_uiProgram)
			:	GLFrameVariableBase( p_uiProgram),
				MatrixFrameVariable<T, Rows, Columns>( * p_pVariable)
		{
		}
		virtual ~GLMatrixFrameVariable()
		{
		}
		virtual void Apply()
		{
			if (m_bChanged)
			{
				_apply<T, Rows, Columns>( m_strName, m_pValues, m_uiOcc);
				m_bChanged = false;
			}
		}
	};
}

#include "GLFrameVariable.inl"

#endif