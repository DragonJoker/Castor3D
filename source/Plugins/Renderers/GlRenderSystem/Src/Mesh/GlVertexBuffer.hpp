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
#ifndef ___GL_VERTEX_BUFFER_OBJECT_H___
#define ___GL_VERTEX_BUFFER_OBJECT_H___

#include "Buffer/GlBuffer.hpp"

#include <Mesh/Buffer/Buffer.hpp>

namespace GlRender
{
	class GlVertexBufferObject
		: public GlBuffer< uint8_t >
	{
	public:
		GlVertexBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, HardwareBufferPtr p_buffer );
		virtual ~GlVertexBufferObject();

		virtual bool Create();
		virtual void Destroy();
		virtual bool Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
		virtual void Cleanup();
		virtual uint8_t * Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags );
		virtual void Unlock();
		virtual bool Bind();
		virtual void Unbind();

	protected:
		uint32_t m_valid;
	};
}

#endif
