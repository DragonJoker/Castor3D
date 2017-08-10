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
#ifndef ___C3DTRS_TestShaderStorageBuffer_H___
#define ___C3DTRS_TestShaderStorageBuffer_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Mesh/Buffer/GpuBuffer.hpp>

namespace TestRender
{
	class TestShaderStorageBuffer
		: public castor3d::GpuBuffer
	{
	public:
		TestShaderStorageBuffer( TestRenderSystem & p_renderSystem );
		virtual ~TestShaderStorageBuffer();
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
		void initialiseStorage( uint32_t p_count
			, castor3d::BufferAccessType p_type
			, castor3d::BufferAccessNature p_nature )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::setBindingPoint
		 */
		void setBindingPoint( uint32_t p_point )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Lock
		 */
		uint8_t * lock( uint32_t p_offset
			, uint32_t p_count
			, castor3d::AccessTypes const & p_flags )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Unlock
		 */
		void unlock()const override;
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
		void copy( GpuBuffer const & p_src
			, uint32_t p_size )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::Upload
		 */
		void upload( uint32_t p_offset
			, uint32_t p_count
			, uint8_t const * p_buffer )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::download
		 */
		void download( uint32_t p_offset
			, uint32_t p_count
			, uint8_t * p_buffer )const override;
	};
}

#endif
