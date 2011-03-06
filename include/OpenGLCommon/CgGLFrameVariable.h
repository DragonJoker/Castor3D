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
#ifndef ___Gl_CgFrameVariable___
#define ___Gl_CgFrameVariable___

#include "Module_GlRender.h"

namespace Castor3D
{
	template <typename Type> inline void ApplyVariable( CGparameter p_cgParameter, const Type * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Count> inline void ApplyVariable( CGparameter p_cgParameter, const Type * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Rows, size_t Columns> inline void ApplyVariable( CGparameter p_cgParameter, const Type * p_pValue, size_t p_uiOcc);

	class CgGlFrameVariableBase
	{
	protected:
		bool m_bPresentInProgram;
		CGparameter m_cgParameter;
		CGprogram m_cgProgram;

	public:
		CgGlFrameVariableBase( CGparameter p_cgParameter, CGprogram p_cgProgram)
			:	m_bPresentInProgram( true),
				m_cgParameter( p_cgParameter),
				m_cgProgram( p_cgProgram)
		{}
		virtual ~CgGlFrameVariableBase()
		{}
		virtual void Apply()=0;

	protected:
		void GetVariableLocation( const char * p_pVarName);
		template <typename Type> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
		template <typename Type, int Count> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
		template <typename Type, int Rows, int Columns> void _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc);
	};

	template <typename T>
	class CgGlOneFrameVariable : public CgOneFrameVariable<T>, public CgGlFrameVariableBase, public MemoryTraced< CgGlOneFrameVariable<T> >
	{
	public:
		CgGlOneFrameVariable( CgOneFrameVariable<T> * p_pVariable)
			:	CgGlFrameVariableBase( p_pVariable->GetParameter(), p_pVariable->GetProgram()),
				CgOneFrameVariable<T>( *p_pVariable)
		{
		}
		virtual ~CgGlOneFrameVariable()
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
	class CgGlPointFrameVariable : public CgPointFrameVariable<T, Count>, public CgGlFrameVariableBase, public MemoryTraced< CgGlPointFrameVariable<T, Count> >
	{
	public:
		CgGlPointFrameVariable( CgPointFrameVariable<T, Count> * p_pVariable)
			:	CgGlFrameVariableBase( p_pVariable->GetParameter(), p_pVariable->GetProgram()),
				CgPointFrameVariable<T, Count>( * p_pVariable)
		{
		}
		virtual ~CgGlPointFrameVariable()
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
	class CgGlMatrixFrameVariable : public CgMatrixFrameVariable<T, Rows, Columns>, public CgGlFrameVariableBase, public MemoryTraced< CgGlMatrixFrameVariable<T, Rows, Columns> >
	{
	public:
		CgGlMatrixFrameVariable( CgMatrixFrameVariable<T, Rows, Columns> * p_pVariable)
			:	CgGlFrameVariableBase( p_pVariable->GetParameter(), p_pVariable->GetProgram()),
				CgMatrixFrameVariable<T, Rows, Columns>( * p_pVariable)
		{
		}
		virtual ~CgGlMatrixFrameVariable()
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
#include "CgGlFrameVariable.inl"
}


#endif
