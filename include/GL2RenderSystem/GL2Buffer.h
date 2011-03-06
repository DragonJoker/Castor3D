/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Gl2_Buffer___
#define ___Gl2_Buffer___

#include "Module_Gl2Render.h"
#include "Gl2RenderSystem.h"

namespace Castor3D
{
	//**************************************************************************
	// Vertex Arrays
	//**************************************************************************

	class C3D_Gl2_API GlVbIndexBuffer : public IndexBuffer
	{
	public:
		GlVbIndexBuffer()
			:	IndexBuffer()
		{
		}

		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags) { return m_buffer; }
		virtual void Unlock(){}
	};

	class C3D_Gl2_API GlVbVertexBuffer : public VertexBuffer
	{
	public:
		GlVbVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiCount)
			:	VertexBuffer( p_pElements, p_uiCount)
		{
		}

		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags) { return m_buffer; }
		virtual void Unlock(){}
	};
}

#endif