/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		~TestIndexBuffer();

		bool Create()override;
		void Destroy()override;
		bool Upload( Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )override;
		uint32_t * Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessType p_flags )override;
		void Unlock()override;
		bool Bind()override;
		void Unbind()override;
		bool Copy( GpuBuffer< uint32_t > const & p_src, uint32_t p_size )override;
		bool Fill( uint32_t const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )override;
	};
}

#endif
