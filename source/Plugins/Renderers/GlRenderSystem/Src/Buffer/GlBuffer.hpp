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

#include "GlRenderSystemPrerequisites.hpp"
#include "Common/GlBindable.hpp"

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
	class GlBuffer
		: public castor3d::GpuBuffer
		, public Bindable<
			std::function< void( int, uint32_t * ) >,
			std::function< void( int, uint32_t const * ) >,
			std::function< void( uint32_t ) > >
	{
		using BindableType = Bindable<
			std::function< void( int, uint32_t * ) >,
			std::function< void( int, uint32_t const * ) >,
			std::function< void( uint32_t ) > >;

	public:
		GlBuffer( GlRenderSystem & renderSystem
			, OpenGl & gl
			, GlBufferTarget target );
		virtual ~GlBuffer();
		/**
		 *\copydoc		castor3d::GpuBuffer::create
		 */
		bool create()override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Destroy
		 */
		void destroy()override;
		/**
		 *\copydoc		castor3d::GpuBuffer::InitialiseStorage
		 */
		void initialiseStorage( uint32_t count
			, castor3d::BufferAccessType type
			, castor3d::BufferAccessNature nature )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::setBindingPoint
		 */
		void setBindingPoint( uint32_t index )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::getBindingPoint
		 */
		uint32_t getBindingPoint()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Bind
		 */
		void bind()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Unbind
		 */
		void unbind()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Copy
		 */
		void copy( castor3d::GpuBuffer const & src
			, uint32_t srcOffset
			, uint32_t dstOffset
			, uint32_t size )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Upload
		 */
		void upload( uint32_t offset
			, uint32_t count
			, uint8_t const * buffer )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Upload
		 */
		void download( uint32_t offset
			, uint32_t count
			, uint8_t * buffer )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Lock
		 */
		uint8_t * lock( uint32_t offset
			, uint32_t count
			, castor3d::AccessTypes const & flags )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Unlock
		 */
		void unlock()const override;

		uint8_t * lock( GlAccessType access )const;

	private:
		GlBufferTarget m_target;
		mutable uint32_t m_bindingPoint{ 0u };
		mutable uint32_t m_allocatedSize{ 0u };
	};
}

#endif
