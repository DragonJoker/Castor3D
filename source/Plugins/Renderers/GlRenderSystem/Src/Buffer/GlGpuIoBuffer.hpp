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
		GlGpuIoBuffer( OpenGl & p_gl, GlRenderSystem * renderSystem, uint32_t p_pixelsSize, GlBufferTarget p_packMode, castor3d::BufferAccessType p_type, castor3d::BufferAccessNature p_nature );
		virtual ~GlGpuIoBuffer();
		bool initialise();

	private:
		virtual bool doInitialise() = 0;

	protected:
		castor3d::BufferAccessType m_accessType;
		castor3d::BufferAccessNature m_accessNature;
		uint32_t m_pixelsSize;
		GlRenderSystem * m_renderSystem;
	};
}

#endif
