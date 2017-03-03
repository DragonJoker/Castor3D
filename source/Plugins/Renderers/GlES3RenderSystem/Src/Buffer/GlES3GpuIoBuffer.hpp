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
#ifndef ___C3DGLES3_GPU_IO_BUFFER_H___
#define ___C3DGLES3_GPU_IO_BUFFER_H___

#include "Buffer/GlES3BufferBase.hpp"

namespace GlES3Render
{
	class GlES3GpuIoBuffer
		: public GlES3BufferBase< uint8_t >
	{
	public:
		GlES3GpuIoBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize, GlES3BufferTarget p_packMode, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature );
		virtual ~GlES3GpuIoBuffer();
		bool Initialise();

	private:
		virtual bool DoInitialise() = 0;

	protected:
		Castor3D::BufferAccessType m_accessType;
		Castor3D::BufferAccessNature m_accessNature;
		uint8_t	* m_pixels;
		uint32_t m_pixelsSize;
		GlES3RenderSystem * m_renderSystem;
	};
}

#endif
