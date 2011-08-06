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
#ifndef ___Dx9_FrameVariable___
#define ___Dx9_FrameVariable___

#include "Module_Dx9Render.hpp"
#include "Dx9RenderSystem.hpp"

namespace Castor3D
{
	template <typename Type> inline void ApplyVariable( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, Type const * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Count> inline void ApplyVariable( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, Type const * p_pValue, size_t p_uiOcc);
	template <typename Type, size_t Rows, size_t Columns> inline void ApplyVariable( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, Type const * p_pValue, size_t p_uiOcc);

	class Dx9FrameVariableBase
	{
	protected:
		D3DXHANDLE m_hDx9Handle;
		ID3DXConstantTable ** m_pConstants;
		bool m_bPresentInProgram;

	public:
		Dx9FrameVariableBase( ID3DXConstantTable ** p_pConstants)
			:	m_hDx9Handle( nullptr)
			,	m_pConstants( p_pConstants)
			,	m_bPresentInProgram( true)
		{}
		virtual ~Dx9FrameVariableBase()
		{}
		virtual void Apply()=0;

	protected:
		void GetVariableLocation( LPCSTR p_pVarName);
		template <typename Type> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc);
		template <typename Type, int Count> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc);
		template <typename Type, int Rows, int Columns> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc);
	};

	template <typename T>
	class Dx9OneFrameVariable : public OneFrameVariable<T>, public Dx9FrameVariableBase, public MemoryTraced< Dx9OneFrameVariable<T> >
	{
	public:
		Dx9OneFrameVariable( Dx9ShaderProgram * p_pProgram, Dx9ShaderObject * p_pObject, size_t p_uiOcc, ID3DXConstantTable ** p_pConstants)
			:	Dx9FrameVariableBase( p_pConstants)
			,	OneFrameVariable<T>( p_pProgram, p_pObject, p_uiOcc)
		{
		}
		Dx9OneFrameVariable( OneFrameVariable<T> * p_pVariable, ID3DXConstantTable * p_pConstants)
			:	Dx9FrameVariableBase( p_pConstants)
			,	OneFrameVariable<T>( *p_pVariable)
		{
		}
		virtual ~Dx9OneFrameVariable()
		{
		}
		virtual void Bind(){}
		virtual void Unbind(){}
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
	class Dx9PointFrameVariable : public PointFrameVariable<T, Count>, public Dx9FrameVariableBase, public MemoryTraced< Dx9PointFrameVariable<T, Count> >
	{
	public:
		Dx9PointFrameVariable( Dx9ShaderProgram * p_pProgram, Dx9ShaderObject * p_pObject, size_t p_uiOcc, ID3DXConstantTable ** p_pConstants)
			:	Dx9FrameVariableBase( p_pConstants)
			,	PointFrameVariable<T, Count>( p_pProgram, p_pObject, p_uiOcc)
		{
		}
		Dx9PointFrameVariable( PointFrameVariable<T, Count> * p_pVariable, ID3DXConstantTable * p_pConstants)
			:	Dx9FrameVariableBase( p_pConstants)
			,	PointFrameVariable<T, Count>( * p_pVariable)
		{
		}
		virtual ~Dx9PointFrameVariable()
		{
		}
		virtual void Bind(){}
		virtual void Unbind(){}
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
	class Dx9MatrixFrameVariable : public MatrixFrameVariable<T, Rows, Columns>, public Dx9FrameVariableBase, public MemoryTraced< Dx9MatrixFrameVariable<T, Rows, Columns> >
	{
	public:
		Dx9MatrixFrameVariable( Dx9ShaderProgram * p_pProgram, Dx9ShaderObject * p_pObject, size_t p_uiOcc, ID3DXConstantTable ** p_pConstants)
			:	Dx9FrameVariableBase( p_pConstants)
			,	MatrixFrameVariable<T, Rows, Columns>( p_pProgram, p_pObject, p_uiOcc)
		{
		}
		Dx9MatrixFrameVariable( MatrixFrameVariable<T, Rows, Columns> * p_pVariable, ID3DXConstantTable * p_pConstants)
			:	Dx9FrameVariableBase( p_pConstants)
			,	MatrixFrameVariable<T, Rows, Columns>( * p_pVariable)
		{
		}
		virtual ~Dx9MatrixFrameVariable()
		{
		}
		virtual void Bind(){}
		virtual void Unbind(){}
		virtual void Apply()
		{
			if (this->m_bChanged)
			{
				_apply<T, Rows, Columns>( this->m_strName, this->m_pValues, this->m_uiOcc);
				this->m_bChanged = false;
			}
		}
	};
#include "Dx9FrameVariable.inl"
}


#endif
