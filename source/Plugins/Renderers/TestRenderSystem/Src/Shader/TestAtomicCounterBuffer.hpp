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
#ifndef ___C3DTRS_TestAtomicCounterBuffer_H___
#define ___C3DTRS_TestAtomicCounterBuffer_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Mesh/Buffer/GpuBuffer.hpp>

namespace TestRender
{
	class TestAtomicCounterBuffer
		: public Castor3D::GpuBuffer< uint32_t >
	{
	public:
		TestAtomicCounterBuffer( TestRenderSystem & p_renderSystem );
		virtual ~TestAtomicCounterBuffer();
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Create
		 */
		bool Create()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Destroy
		 */
		void Destroy()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::InitialiseStorage
		 */
		void InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::SetBindingPoint
		 */
		void SetBindingPoint( uint32_t p_point )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Lock
		 */
		uint32_t * Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessTypes const & p_flags )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Unlock
		 */
		void Unlock()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Bind
		 */
		void Bind()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Unbind
		 */
		void Unbind()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Copy
		 */
		void Copy( GpuBuffer< uint32_t > const & p_src, uint32_t p_size )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Upload
		 */
		void Upload( uint32_t p_offset, uint32_t p_count, uint32_t const * p_buffer )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< uint8_t >::Download
		 */
		void Download( uint32_t p_offset, uint32_t p_count, uint32_t * p_buffer )const override;
	};
}

#endif
