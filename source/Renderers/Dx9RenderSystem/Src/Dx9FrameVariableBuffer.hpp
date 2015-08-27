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
#ifndef ___DX9_FRAME_VARIABLE_BUFFER_H___
#define ___DX9_FRAME_VARIABLE_BUFFER_H___

#include "Dx9RenderSystemPrerequisites.hpp"

#include <FrameVariableBuffer.hpp>

namespace Dx9Render
{
	struct DxVariableApplyerBase
	{
		virtual void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Castor3D::FrameVariableSPtr p_pVariable ) = 0;
	};

	DECLARE_SMART_PTR( DxVariableApplyerBase );

	template< typename Type											> inline void ApplyVariable( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Type const * p_pValue, uint32_t p_uiOcc );
	template< typename Type, std::size_t Count						> inline void ApplyVariable( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Type const * p_pValue, uint32_t p_uiOcc );
	template< typename Type, std::size_t Rows, std::size_t Columns	> inline void ApplyVariable( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Type const * p_pValue, uint32_t p_uiOcc );

	class DxFrameVariableBuffer
		:	public Castor3D::FrameVariableBuffer
	{
	public:
		DxFrameVariableBuffer( Castor::String const & p_strName, DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBuffer();

	private:
		virtual Castor3D::FrameVariableSPtr DoCreateVariable( Castor3D::ShaderProgramBase * p_pProgram, Castor3D::eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc = 1 );
		virtual bool DoInitialise( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();

	private:
		DxRenderSystem * m_pDxRenderSystem;
		DxShaderProgram * m_pShaderProgram;
	};
}

#endif
