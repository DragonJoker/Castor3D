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
#ifndef ___C3DTRS_TestBuffer_H___
#define ___C3DTRS_TestBuffer_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Mesh/Buffer/Buffer.hpp>

namespace TestRender
{
	class TestBuffer
		: public castor3d::GpuBuffer
	{
	public:
		TestBuffer( TestRenderSystem & renderSystem
			, castor3d::BufferType type );
		~TestBuffer();
		/**
		 *\copydoc		castor3d::GpuBuffer::create
		 */
		bool create()override;
		/**
		 *\copydoc		castor3d::GpuBuffer::destroy
		 */
		void destroy()override;
		/**
		 *\copydoc		castor3d::GpuBuffer::setBindingPoint
		 */
		void setBindingPoint( uint32_t point )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::getBindingPoint
		 */
		uint32_t getBindingPoint()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::lock
		 */
		uint8_t * lock( uint32_t offset
			, uint32_t count
			, castor3d::AccessTypes const & flags )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::unlock
		 */
		void unlock()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::bind
		 */
		void bind()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::unbind
		 */
		void unbind()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::copy
		 */
		void copy( castor3d::GpuBuffer const & src
			, uint32_t srcOffset
			, uint32_t dstOffset
			, uint32_t size )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::upload
		 */
		void upload( uint32_t offset
			, uint32_t count
			, uint8_t const * buffer )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::download
		 */
		void download( uint32_t offset
			, uint32_t count
			, uint8_t * buffer )const override;

	private:
		/**
		 *\copydoc		castor3d::GpuBuffer::doInitialiseStorage
		 */
		void doInitialiseStorage( uint32_t count
			, castor3d::BufferAccessType type
			, castor3d::BufferAccessNature nature )const override;

	private:
		castor3d::BufferType m_type;
	};
}

#endif
