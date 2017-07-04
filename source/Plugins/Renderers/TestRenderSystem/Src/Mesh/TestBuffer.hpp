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
		: public Castor3D::GpuBuffer
	{
	public:
		TestBuffer( TestRenderSystem & p_renderSystem
			, Castor3D::BufferType p_type );
		~TestBuffer();
		/**
		 *\copydoc		Castor3D::GpuBuffer::Create
		 */
		bool Create()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Destroy
		 */
		void Destroy()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::InitialiseStorage
		 */
		void InitialiseStorage( uint32_t p_count
			, Castor3D::BufferAccessType p_type
			, Castor3D::BufferAccessNature p_nature )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::SetBindingPoint
		 */
		void SetBindingPoint( uint32_t p_point )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::GetBindingPoint
		 */
		uint32_t GetBindingPoint()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Lock
		 */
		uint8_t * Lock( uint32_t p_offset
			, uint32_t p_count
			, Castor3D::AccessTypes const & p_flags )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Unlock
		 */
		void Unlock()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Bind
		 */
		void Bind()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Unbind
		 */
		void Unbind()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Copy
		 */
		void Copy( Castor3D::GpuBuffer const & p_src
			, uint32_t p_size )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Upload
		 */
		void Upload( uint32_t p_offset
			, uint32_t p_count
			, uint8_t const * p_buffer )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer::Download
		 */
		void Download( uint32_t p_offset
			, uint32_t p_count
			, uint8_t * p_buffer )const override;

	private:
		Castor3D::BufferType m_type;
	};
}

#endif
