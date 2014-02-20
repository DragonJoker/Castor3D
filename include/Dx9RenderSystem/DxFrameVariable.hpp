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
#include "DxRenderSystem.hpp"
#include "DxTextureRenderer.hpp"

namespace Dx9Render
{
	struct DxVariableApplyerBase
	{
		virtual void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Castor3D::FrameVariableSPtr p_pVariable )=0;
	};
	template< typename Type > struct OneVariableApplyer;
	template< typename Type, uint32_t Count > struct PointVariableApplyer;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableApplyer;

	DECLARE_SMART_PTR( DxVariableApplyerBase );

	template< typename Type											> inline void ApplyVariable( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Type const * p_pValue, uint32_t p_uiOcc );
	template< typename Type, std::size_t Count						> inline void ApplyVariable( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Type const * p_pValue, uint32_t p_uiOcc );
	template< typename Type, std::size_t Rows, std::size_t Columns	> inline void ApplyVariable( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Type const * p_pValue, uint32_t p_uiOcc );

	class DxFrameVariableBuffer : public Castor3D::FrameVariableBuffer
	{
	private:
		DxRenderSystem *	m_pDxRenderSystem;
		DxShaderProgram *	m_pShaderProgram;

	public:
		DxFrameVariableBuffer( Castor::String const & p_strName, DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBuffer();

	private:
		virtual Castor3D::FrameVariableSPtr DoCreateVariable( Castor3D::ShaderProgramBase * p_pProgram, Castor3D::eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc=1 );
		virtual bool DoInitialise( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();
	};

	class DxFrameVariableBase
	{
	protected:
		D3DXHANDLE				m_d3dxHandle;
		bool					m_bPresentInProgram;
		DxRenderSystem *		m_pRenderSystem;

	public:
		DxFrameVariableBase( DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBase();

		virtual void Apply()=0;

	protected:
		void GetVariableLocation( LPCSTR p_pVarName, ID3DXEffect * p_pEffect );
		template< typename Type							> void DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc);
		template< typename Type, int Count				> void DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc);
		template< typename Type, int Rows, int Columns	> void DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc);
	};

	template< typename T >
	class DxOneFrameVariable : public Castor3D::OneFrameVariable< T >, public DxFrameVariableBase
	{
	public:
		DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc );
		DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::OneFrameVariable< T > * p_pVariable );
		virtual ~DxOneFrameVariable();

		virtual bool Initialise	();
		virtual void Cleanup	();
		virtual void Bind		();
		virtual void Unbind		();
		virtual void Apply		();
	};

	template< typename T, std::size_t Count >
	class DxPointFrameVariable : public Castor3D::PointFrameVariable< T, Count >, public DxFrameVariableBase
	{
	public:
		DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc );
		DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::PointFrameVariable< T, Count > * p_pVariable );
		virtual ~DxPointFrameVariable();

		virtual bool Initialise	();
		virtual void Cleanup	();
		virtual void Bind		();
		virtual void Unbind		();
		virtual void Apply		();
	};

	template< typename T, std::size_t Rows, std::size_t Columns >
	class DxMatrixFrameVariable : public Castor3D::MatrixFrameVariable< T, Rows, Columns >, public DxFrameVariableBase
	{
	public:
		DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc );
		DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::MatrixFrameVariable< T, Rows, Columns > * p_pVariable );
		virtual ~DxMatrixFrameVariable();

		virtual bool Initialise	();
		virtual void Cleanup	();
		virtual void Bind		();
		virtual void Unbind		();
		virtual void Apply		();
	};
#include "DxFrameVariable.inl"
}


#endif
