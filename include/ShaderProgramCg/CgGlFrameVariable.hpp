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
#ifndef ___Cg_GlFrameVariable___
#define ___Cg_GlFrameVariable___

#include "Module_CgShader.hpp"

#if defined( Cg_OpenGL )
#include "CgFrameVariable.hpp"
#include <GlRenderSystem/GlTextureRenderer.hpp>

namespace CgShader
{
	class CgGlFrameVariableBase
	{
	protected:
		bool m_bPresentInProgram;
		CGparameter m_cgParameter;

	public:
		CgGlFrameVariableBase();
		virtual ~CgGlFrameVariableBase();

		virtual void Apply()=0;

	protected:
		void GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram);
		template< typename Type							> void DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram );
		template< typename Type, int Count				> void DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram );
		template< typename Type, int Rows, int Columns	> void DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram );
	};

	template< typename T >
	class CgGlOneFrameVariable : public CgOneFrameVariable< T >, public CgGlFrameVariableBase
	{
	public:
		CgGlOneFrameVariable( CgGlShaderProgram * p_pProgram, uint32_t p_uiOcc );
		CgGlOneFrameVariable( CgOneFrameVariable< T > * p_pVariable );
		virtual ~CgGlOneFrameVariable();
		
		virtual bool Initialise	(){return true; }
		virtual void Cleanup	(){}
		virtual void Bind	();
		virtual void Apply	();
	};

	template< typename T, uint32_t Count >
	class CgGlPointFrameVariable : public CgPointFrameVariable< T, Count >, public CgGlFrameVariableBase
	{
	public:
		CgGlPointFrameVariable( CgGlShaderProgram * p_pProgram, uint32_t p_uiOcc );
		CgGlPointFrameVariable( CgPointFrameVariable< T, Count > * p_pVariable );
		virtual ~CgGlPointFrameVariable();
		
		virtual bool Initialise	(){return true; }
		virtual void Cleanup	(){}
		virtual void Bind	();
		virtual void Apply	();
	};

	template< typename T, uint32_t Rows, uint32_t Columns >
	class CgGlMatrixFrameVariable : public CgMatrixFrameVariable< T, Rows, Columns >, public CgGlFrameVariableBase
	{
	public:
		CgGlMatrixFrameVariable( CgGlShaderProgram * p_pProgram, uint32_t p_uiOcc );
		CgGlMatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > * p_pVariable );
		virtual ~CgGlMatrixFrameVariable();
		
		virtual bool Initialise	(){return true; }
		virtual void Cleanup	(){}
		virtual void Bind	();
		virtual void Apply	();
	};
#include "CgGlFrameVariable.inl"
}

#endif
#endif
