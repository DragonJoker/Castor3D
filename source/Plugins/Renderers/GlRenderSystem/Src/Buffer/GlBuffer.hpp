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
	\brief 		Classe implémentant les VBO OpenGL.
	*/
	template< typename T >
	class GlBuffer
		: public Castor3D::GpuBuffer< T >
		, public Holder
	{
	protected:
		using HardwareBufferPtr = typename Castor3D::GpuBuffer< T >::HardwareBufferPtr;

	public:
		GlBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, GlBufferTarget p_target, HardwareBufferPtr p_buffer );
		virtual ~GlBuffer();
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Create
		 */
		bool Create()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Destroy
		 */
		void Destroy()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Initialise
		 */
		bool Upload( Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Bind
		 */
		bool Bind()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Unbind
		 */
		void Unbind()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Copy
		 */
		bool Copy( GpuBuffer< T > const & p_src, uint32_t p_size )override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Fill
		 */
		bool Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Lock
		 */
		T * Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessType p_flags )override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Unlock
		 */
		void Unlock()override;

		inline uint32_t GetGlName()const
		{
			return m_glBuffer.GetGlName();
		}
		HardwareBufferPtr GetCpuBuffer()const
		{
			return m_buffer;
		}

	protected:
		GlBufferBase< T > m_glBuffer;
		HardwareBufferPtr m_buffer;
	};
}

#include "GlBuffer.inl"

#endif
