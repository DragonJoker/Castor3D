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
#ifndef ___GL_BUFFER_H___
#define ___GL_BUFFER_H___

#include "Buffer/GlBufferBase.hpp"

#include <Mesh/Buffer/GpuBuffer.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version 	0.6.1.0
	\date 		03/01/2011
	\~english
	\brief 		Class implementing OpenGL VBO.
	\~french
	\brief 		Classe implàmentant les VBO OpenGL.
	*/
	template< typename T >
	class GlBuffer
		: public Castor3D::GpuBuffer< T >
		, public Holder
	{
	protected:
		typedef typename Castor3D::GpuBuffer< T >::HardwareBufferPtr HardwareBufferPtr;

	public:
		GlBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, eGL_BUFFER_TARGET p_target, HardwareBufferPtr p_buffer );
		virtual ~GlBuffer();

		virtual bool Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );

		inline uint32_t GetGlName()const
		{
			return m_glBuffer.GetGlName();
		}
		HardwareBufferPtr GetCpuBuffer()const
		{
			return m_buffer;
		}

	protected:
		virtual bool DoCreate();
		virtual void DoDestroy();
		virtual bool DoInitialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();
		virtual bool DoFill( T * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
		virtual T * DoLock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags );
		virtual T * DoLock( eGL_LOCK p_access );
		virtual void DoUnlock();

	protected:
		GlBufferBase< T > m_glBuffer;
		HardwareBufferPtr m_buffer;
	};
}

#include "GlBuffer.inl"

#endif
