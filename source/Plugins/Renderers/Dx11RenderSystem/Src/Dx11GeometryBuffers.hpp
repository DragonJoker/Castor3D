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
#ifndef ___DX11_GEOMETRY_BUFFERS_H___
#define ___DX11_GEOMETRY_BUFFERS_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <GeometryBuffers.hpp>

namespace Dx11Render
{
	class DxGeometryBuffers
		:	public Castor3D::GeometryBuffers
	{
	public:
		DxGeometryBuffers( DxRenderSystem * p_renderSystem, Castor3D::VertexBufferUPtr p_pVertexBuffer, Castor3D::IndexBufferUPtr p_pIndexBuffer, Castor3D::MatrixBufferUPtr p_pMatrixBuffer );
		virtual ~DxGeometryBuffers();

		virtual bool Draw( Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index );
		virtual bool DrawInstanced( Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index, uint32_t p_count );

	private:
		virtual bool DoCreate();
		virtual void DoDestroy();
		virtual bool DoInitialise();
		virtual void DoCleanup();

	private:
		DxRenderSystem * m_renderSystem;
	};
}

#endif
