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
#ifndef ___Dx11_FrameVariable___
#define ___Dx11_FrameVariable___

#include "Module_DxRender.hpp"
#include "DxRenderSystem.hpp"
#include "DxTextureRenderer.hpp"

namespace Dx11Render
{
	template< typename Type > struct OneVariableApplier;
	template< typename Type, uint32_t Count > struct PointVariableApplier;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableApplier;

	class DxFrameVariableBuffer : public Castor3D::FrameVariableBuffer
	{
	private:
		DxRenderSystem *	m_pDxRenderSystem;
		ID3D11Buffer *		m_pDxBuffer;
		DxShaderProgram *	m_pShaderProgram;

	public:
		DxFrameVariableBuffer( Castor::String const & p_strName, DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBuffer();

		virtual bool Bind( uint32_t p_uiIndex );
		virtual void Unbind( uint32_t p_uiIndex );

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
		std::string				m_stdstrName;
		bool					m_bPresentInProgram;
		DxRenderSystem *		m_pRenderSystem;

	public:
		DxFrameVariableBase( DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBase();
		
		virtual void Apply()=0;

	protected:
		template< typename Type										> void DoApply( DxShaderProgram & p_program, Type * p_pValue );
		template< typename Type, uint32_t Count						> void DoApply( DxShaderProgram & p_program, Type * p_pValue );
		template< typename Type, uint32_t Rows, uint32_t Columns	> void DoApply( DxShaderProgram & p_program, Type * p_pValue );
	};

	template< typename T >
	class DxOneFrameVariable : public Castor3D::OneFrameVariable< T >, public DxFrameVariableBase
	{
	public:
		DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc );
		DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::OneFrameVariable< T > * p_pVariable );
		virtual ~DxOneFrameVariable();
		
		virtual bool Initialise	(){ return true; }
		virtual void Cleanup	(){}
		virtual void Bind		();
		virtual void Unbind		();
		virtual void Apply		();
	};

	template< typename T, uint32_t Count >
	class DxPointFrameVariable : public Castor3D::PointFrameVariable< T, Count >, public DxFrameVariableBase
	{
	public:
		DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc );
		DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::PointFrameVariable< T, Count > * p_pVariable );
		virtual ~DxPointFrameVariable();
		
		virtual bool Initialise	(){ return true; }
		virtual void Cleanup	(){}
		virtual void Bind		();
		virtual void Unbind		();
		virtual void Apply		();
	};

	template< typename T, uint32_t Rows, uint32_t Columns >
	class DxMatrixFrameVariable : public Castor3D::MatrixFrameVariable< T, Rows, Columns >, public DxFrameVariableBase
	{
	public:
		DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc );
		DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::MatrixFrameVariable< T, Rows, Columns > * p_pVariable );
		virtual ~DxMatrixFrameVariable();
		
		virtual bool Initialise	(){ return true; }
		virtual void Cleanup	(){}
		virtual void Bind		();
		virtual void Unbind		();
		virtual void Apply		();
	};
#include "DxFrameVariable.inl"
}


#endif
