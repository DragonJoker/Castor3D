/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___Dx9_CgFrameVariable___
#define ___Dx9_CgFrameVariable___

#include "Module_Dx9Render.hpp"
#include "Dx9RenderSystem.hpp"

namespace Castor3D
{
	template <typename Type> inline void ApplyVariable( CGparameter p_cgParameter, Type const * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Count> inline void ApplyVariable( CGparameter p_cgParameter, Type const * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Rows, size_t Columns> inline void ApplyVariable( CGparameter p_cgParameter, Type const * p_pValue, size_t p_uiOcc);

	class CgDx9FrameVariableBase
	{
	protected:
		bool m_bPresentInProgram;
		CGparameter m_cgParameter;

	public:
		CgDx9FrameVariableBase()
			:	m_bPresentInProgram( true)
			,	m_cgParameter( nullptr)
		{
		}
		virtual ~CgDx9FrameVariableBase()
		{
		}
		virtual void Apply()=0;

	protected:
		void GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram);
		template <typename Type> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram);
		template <typename Type, int Count> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram);
		template <typename Type, int Rows, int Columns> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram);
	};

	template <typename T>
	class CgDx9OneFrameVariable : public CgOneFrameVariable<T>, public CgDx9FrameVariableBase, public MemoryTraced< CgDx9OneFrameVariable<T> >
	{
	public:
		CgDx9OneFrameVariable( CgDx9ShaderProgram * p_pProgram, CgDx9ShaderObject * p_pObject, CGprogram p_cgProgram, size_t p_uiOcc)
			:	CgDx9FrameVariableBase()
			,	CgOneFrameVariable<T>( p_pProgram, p_pObject, p_cgProgram, p_uiOcc)
		{
		}
		CgDx9OneFrameVariable( CgOneFrameVariable<T> * p_pVariable)
			:	CgDx9FrameVariableBase( p_pVariable->GetParameter(), p_pVariable->GetProgram())
			,	CgOneFrameVariable<T>( *p_pVariable)
		{
		}
		virtual ~CgDx9OneFrameVariable()
		{
		}
		virtual void Bind()
		{
		}
		virtual void Apply()
		{
			if (this->m_bChanged)
			{
				_apply<T>( this->m_strName, this->m_tValue, this->m_uiOcc, this->m_cgProgram);
				this->m_bChanged = false;
			}
		}
	};

	template <typename T, size_t Count>
	class CgDx9PointFrameVariable : public CgPointFrameVariable<T, Count>, public CgDx9FrameVariableBase, public MemoryTraced< CgDx9PointFrameVariable<T, Count> >
	{
	public:
		CgDx9PointFrameVariable( CgDx9ShaderProgram * p_pProgram, CgDx9ShaderObject * p_pObject, CGprogram p_cgProgram, size_t p_uiOcc)
			:	CgDx9FrameVariableBase()
			,	CgPointFrameVariable<T, Count>( p_pProgram, p_pObject, p_cgProgram, p_uiOcc)
		{
		}
		CgDx9PointFrameVariable( CgPointFrameVariable<T, Count> * p_pVariable)
			:	CgDx9FrameVariableBase( p_pVariable->GetParameter(), p_pVariable->GetProgram())
			,	CgPointFrameVariable<T, Count>( * p_pVariable)
		{
		}
		virtual ~CgDx9PointFrameVariable()
		{
		}
		virtual void Bind()
		{
		}
		virtual void Apply()
		{
			if (this->m_bChanged)
			{
				_apply<T, Count>( this->m_strName, this->m_pValues, this->m_uiOcc, this->m_cgProgram);
				this->m_bChanged = false;
			}
		}
	};

	template <typename T, size_t Rows, size_t Columns>
	class CgDx9MatrixFrameVariable : public CgMatrixFrameVariable<T, Rows, Columns>, public CgDx9FrameVariableBase, public MemoryTraced< CgDx9MatrixFrameVariable<T, Rows, Columns> >
	{
	public:
		CgDx9MatrixFrameVariable( CgDx9ShaderProgram * p_pProgram, CgDx9ShaderObject * p_pObject, CGprogram p_cgProgram, size_t p_uiOcc)
			:	CgDx9FrameVariableBase()
			,	CgMatrixFrameVariable<T, Rows, Columns>( p_pProgram, p_pObject, p_cgProgram, p_uiOcc)
		{
		}
		CgDx9MatrixFrameVariable( CgMatrixFrameVariable<T, Rows, Columns> * p_pVariable)
			:	CgDx9FrameVariableBase( p_pVariable->GetParameter(), p_pVariable->GetProgram())
			,	CgMatrixFrameVariable<T, Rows, Columns>( * p_pVariable)
		{
		}
		virtual ~CgDx9MatrixFrameVariable()
		{
		}
		virtual void Bind()
		{
		}
		virtual void Apply()
		{
			if (this->m_bChanged)
			{
				_apply<T, Rows, Columns>( this->m_strName, this->m_pValues, this->m_uiOcc, this->m_cgProgram);
				this->m_bChanged = false;
			}
		}
	};
#include "CgDx9FrameVariable.inl"
}


#endif
