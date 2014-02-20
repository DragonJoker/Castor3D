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
#ifndef ___Cg_Dx11FrameVariable___
#define ___Cg_Dx11FrameVariable___

#include "Module_CgShader.hpp"

#if defined( Cg_Direct3D11 )
#include "CgFrameVariable.hpp"
#include <Dx11RenderSystem/DxTextureRenderer.hpp>

namespace CgShader
{
	class CgD3D11FrameVariableBase
	{
	protected:
		bool m_bPresentInProgram;
		CGparameter m_cgParameter;

	public:
		CgD3D11FrameVariableBase();
		virtual ~CgD3D11FrameVariableBase();

		virtual void Apply()=0;

	protected:
		void GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram);
		template< typename Type										> void DoApply( CgD3D11ShaderProgram & p_program, CgD3D11OneFrameVariable< Type > * p_pVariable );
		template< typename Type, uint32_t Count						> void DoApply( CgD3D11ShaderProgram & p_program, CgD3D11PointFrameVariable< Type, Count > * p_pVariable );
		template< typename Type, uint32_t Rows, uint32_t Columns	> void DoApply( CgD3D11ShaderProgram & p_program, CgD3D11MatrixFrameVariable< Type, Rows, Columns > * p_pVariable );
		template< typename Type							> void DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram);
		template< typename Type, int Count				> void DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram);
		template< typename Type, int Rows, int Columns	> void DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram);
	};

	template< typename T >
	class CgD3D11OneFrameVariable : public CgOneFrameVariable< T >, public CgD3D11FrameVariableBase
	{
	public:
		CgD3D11OneFrameVariable( CgD3D11ShaderProgram * p_pProgram, uint32_t p_uiOcc );
		CgD3D11OneFrameVariable( CgOneFrameVariable< T > * p_pVariable );
		virtual ~CgD3D11OneFrameVariable();

		virtual bool Initialise	(){return true; }
		virtual void Cleanup	(){}
		virtual void Bind		();
		virtual void Apply		();
	};

	template< typename T, uint32_t Count >
	class CgD3D11PointFrameVariable : public CgPointFrameVariable< T, Count >, public CgD3D11FrameVariableBase
	{
	public:
		CgD3D11PointFrameVariable( CgD3D11ShaderProgram * p_pProgram, uint32_t p_uiOcc );
		CgD3D11PointFrameVariable( CgPointFrameVariable< T, Count > * p_pVariable );
		virtual ~CgD3D11PointFrameVariable();

		virtual bool Initialise	(){return true; }
		virtual void Cleanup	(){}
		virtual void Bind		();
		virtual void Apply		();
	};

	template< typename T, uint32_t Rows, uint32_t Columns >
	class CgD3D11MatrixFrameVariable : public CgMatrixFrameVariable< T, Rows, Columns >, public CgD3D11FrameVariableBase
	{
	public:
		CgD3D11MatrixFrameVariable( CgD3D11ShaderProgram * p_pProgram, uint32_t p_uiOcc );
		CgD3D11MatrixFrameVariable( CgMatrixFrameVariable<T, Rows, Columns> * p_pVariable );
		virtual ~CgD3D11MatrixFrameVariable();

		virtual bool Initialise	(){return true; }
		virtual void Cleanup	(){}
		virtual void Bind		();
		virtual void Apply		();
	};
#include "CgDx11FrameVariable.inl"
}

#endif
#endif
