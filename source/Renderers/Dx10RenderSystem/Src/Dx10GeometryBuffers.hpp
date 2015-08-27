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
#ifndef ___DX10_GEOMETRY_BUFFERS_H___
#define ___DX10_GEOMETRY_BUFFERS_H___

#include "Dx10BufferObjectBase.hpp"

#include <GeometryBuffers.hpp>

namespace Dx10Render
{
	class DxGeometryBuffers
		: public Castor3D::GeometryBuffers
	{
	public:
		DxGeometryBuffers( DxRenderSystem * p_pRenderSystem, Castor3D::VertexBufferUPtr p_pVertexBuffer, Castor3D::IndexBufferUPtr p_pIndexBuffer, Castor3D::MatrixBufferUPtr p_pMatrixBuffer );
		virtual ~DxGeometryBuffers();

		virtual bool Draw( Castor3D::eTOPOLOGY p_ePrimitiveType, Castor3D::ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex );
		virtual bool DrawInstanced( Castor3D::eTOPOLOGY p_ePrimitiveType, Castor3D::ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount );
		virtual bool Initialise();
		virtual void Cleanup();

	private:
		DxRenderSystem * m_pRenderSystem;
	};
}

#endif
