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
#ifndef ___GL_GPU_IO_BUFFER_H___
#define ___GL_GPU_IO_BUFFER_H___

#include "Buffer/GlBufferBase.hpp"

namespace GlRender
{
	class GlGpuIoBuffer
		: public GlBufferBase< uint8_t >
	{
	public:
		GlGpuIoBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize, eGL_BUFFER_TARGET p_packMode, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature );
		virtual ~GlGpuIoBuffer();

		virtual bool Activate();
		virtual void Deactivate();
		bool Fill( uint8_t * p_buffer, ptrdiff_t p_size );

		virtual bool Initialise() = 0;

	protected:
		Castor3D::BufferAccessType m_accessType;
		Castor3D::BufferAccessNature m_accessNature;
		uint8_t	* m_pixels;
		uint32_t m_pixelsSize;
		GlRenderSystem * m_renderSystem;
	};
}

#endif
