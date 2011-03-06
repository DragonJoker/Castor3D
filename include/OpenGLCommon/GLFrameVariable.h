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
#ifndef ___Gl_FrameVariable___
#define ___Gl_FrameVariable___

#include "Module_GlRender.h"

namespace Castor3D
{
	template <typename Type> inline void ApplyVariable( int p_iGlIndex, const Type * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Count> inline void ApplyVariable( int p_iGlIndex, const Type * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Rows, size_t Columns> inline void ApplyVariable( int p_iGlIndex, const Type * p_pValue, size_t p_uiOcc);

	class GlFrameVariableBase
	{
	protected:
		int m_glIndex;
		GLuint * m_uiParentProgram;
		bool m_bPresentInProgram;

	public:
		GlFrameVariableBase( GLuint * p_uiProgram)
			:	m_glIndex( -1),
				m_uiParentProgram( p_uiProgram),
				m_bPresentInProgram( true)
		{}
		virtual ~GlFrameVariableBase()
		{}
		virtual void Apply()=0;

	protected:
		void GetVariableLocation( const GLchar * p_pVarName);
		template <typename Type> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
		template <typename Type, int Count> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
		template <typename Type, int Rows, int Columns> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
	};

	template <typename T>
	class GlOneFrameVariable : public OneFrameVariable<T>, public GlFrameVariableBase, public MemoryTraced< GlOneFrameVariable<T> >
	{
	public:
		GlOneFrameVariable( OneFrameVariable<T> * p_pVariable, GLuint * p_uiProgram)
			:	GlFrameVariableBase( p_uiProgram),
				OneFrameVariable<T>( *p_pVariable)
		{
		}
		virtual ~GlOneFrameVariable()
		{
		}
		virtual void Apply()
		{
			if (this->m_bChanged)
			{
				_apply<T>( this->m_strName, this->m_tValue, this->m_uiOcc);
				this->m_bChanged = false;
			}
		}
	};

	template <typename T, size_t Count>
	class GlPointFrameVariable : public PointFrameVariable<T, Count>, public GlFrameVariableBase, public MemoryTraced< GlPointFrameVariable<T, Count> >
	{
	public:
		GlPointFrameVariable( PointFrameVariable<T, Count> * p_pVariable, GLuint * p_uiProgram)
			:	GlFrameVariableBase( p_uiProgram),
				PointFrameVariable<T, Count>( * p_pVariable)
		{
		}
		virtual ~GlPointFrameVariable()
		{
		}
		virtual void Apply()
		{
			if (this->m_bChanged)
			{
				_apply<T, Count>( this->m_strName, this->m_pValues, this->m_uiOcc);
				this->m_bChanged = false;
			}
		}
	};

	template <typename T, size_t Rows, size_t Columns>
	class GlMatrixFrameVariable : public MatrixFrameVariable<T, Rows, Columns>, public GlFrameVariableBase, public MemoryTraced< GlMatrixFrameVariable<T, Rows, Columns> >
	{
	public:
		GlMatrixFrameVariable( MatrixFrameVariable<T, Rows, Columns> * p_pVariable, GLuint * p_uiProgram)
			:	GlFrameVariableBase( p_uiProgram),
				MatrixFrameVariable<T, Rows, Columns>( * p_pVariable)
		{
		}
		virtual ~GlMatrixFrameVariable()
		{
		}
		virtual void Apply()
		{
			if (this->m_bChanged)
			{
				_apply<T, Rows, Columns>( this->m_strName, this->m_pValues, this->m_uiOcc);
				this->m_bChanged = false;
			}
		}
	};
#include "GlFrameVariable.inl"
}


#endif
