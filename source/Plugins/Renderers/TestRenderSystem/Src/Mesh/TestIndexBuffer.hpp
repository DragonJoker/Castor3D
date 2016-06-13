/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___TRS_INDEX_BUFFER_OBJECT_H___
#define ___TRS_INDEX_BUFFER_OBJECT_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Mesh/Buffer/Buffer.hpp>

namespace TestRender
{
	class TestIndexBuffer
		: public Castor3D::GpuBuffer< uint32_t >
	{
	protected:
		using HardwareBufferPtr = Castor3D::GpuBuffer< uint32_t >::HardwareBufferPtr;

	public:
		TestIndexBuffer( TestRenderSystem & p_renderSystem );
		virtual ~TestIndexBuffer();

		virtual bool Create();
		virtual void Destroy();
		virtual bool Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
		virtual bool AttachTo( Castor3D::ShaderProgramSPtr p_program );
		virtual void Cleanup();
		virtual uint32_t * Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags );
		virtual void Unlock();
		virtual bool Bind();
		virtual void Unbind();
		virtual bool Fill( uint32_t const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
	};
}

#endif
