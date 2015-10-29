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
#ifndef ___DX11_MATRIX_BUFFER_H___
#define ___DX11_MATRIX_BUFFER_H___

#include "Dx11BufferObject.hpp"

namespace Dx11Render
{
	class DxMatrixBuffer
		:	public DxBufferObject< real, ID3D11Buffer >
	{
	public:
		DxMatrixBuffer( DxRenderSystem & p_renderSystem, HardwareBufferPtr p_buffer );
		virtual ~DxMatrixBuffer();

		virtual bool Create();
		virtual void Destroy();
		virtual bool Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void Cleanup();
		virtual bool Bind( uint32_t p_uiCount );
		virtual void Unbind();
		virtual real * Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void Unlock();

	private:
		virtual bool Bind()
		{
			return false;
		}

	private:
		ID3D11InputLayout * m_pDxDeclaration;
		DxShaderProgramWPtr m_pProgram;
	};
}

#endif
