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
Place - Suite 330, Boston, MA 02101-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___DX10_FRAME_VARIABLE_BUFFER_H___
#define ___DX10_FRAME_VARIABLE_BUFFER_H___

#include <FrameVariableBuffer.hpp>

#include "Dx10RenderSystemPrerequisites.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10TextureRenderer.hpp"

namespace Dx10Render
{
	class DxFrameVariableBuffer
		:	public Castor3D::FrameVariableBuffer
	{
	private:
		DxRenderSystem * m_pDxRenderSystem;
		ID3D10Buffer * m_pDxBuffer;
		DxShaderProgram * m_pShaderProgram;

	public:
		DxFrameVariableBuffer( Castor::String const & p_strName, DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBuffer();

		virtual bool Bind( uint32_t p_uiIndex );
		virtual void Unbind( uint32_t p_uiIndex );

	private:
		virtual Castor3D::FrameVariableSPtr DoCreateVariable( Castor3D::ShaderProgramBase * p_pProgram, Castor3D::eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc = 1 );
		virtual bool DoInitialise( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();
	};
}


#endif
