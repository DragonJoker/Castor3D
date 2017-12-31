/* See LICENSE file in root folder */
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
