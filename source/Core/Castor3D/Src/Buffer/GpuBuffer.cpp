#include "Buffer/GpuBuffer.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	GpuBuffer::GpuBuffer()
	{
	}

	GpuBuffer::~GpuBuffer()
	{
	}

	void GpuBuffer::initialiseStorage( renderer::Device const & device
		, uint32_t level
		, uint32_t minBlockSize
		, renderer::BufferTargets targets
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		m_allocator = std::make_unique< GpuBufferBuddyAllocator >( level, minBlockSize );
		doInitialiseStorage( device
			, uint32_t( m_allocator->getSize() )
			, targets
			, memoryFlags );
	}

	bool GpuBuffer::hasAvailable( size_t size )const
	{
		return m_allocator->hasAvailable( size );
	}

	uint32_t GpuBuffer::allocate( size_t size )
	{
		return m_allocator->allocate( size );
	}

	void GpuBuffer::deallocate( uint32_t offset )
	{
		m_allocator->deallocate( offset );
	}

	uint8_t * GpuBuffer::lock( uint32_t offset
		, uint32_t size
		, renderer::MemoryMapFlags const & flags )const
	{
		return m_buffer->getBuffer().lock( offset, size, flags );
	}

	void GpuBuffer::flush( uint32_t offset
		, uint32_t size )const
	{
		return m_buffer->getBuffer().flush( offset, size );
	}

	void GpuBuffer::invalidate( uint32_t offset
		, uint32_t size )const
	{
		return m_buffer->getBuffer().invalidate( offset, size );
	}

	void GpuBuffer::unlock()const
	{
		m_buffer->getBuffer().unlock();
	}

	void GpuBuffer::copy( renderer::CommandBuffer const & commandBuffer
		, GpuBuffer const & src
		, uint32_t srcOffset
		, uint32_t dstOffset
		, uint32_t size )const
	{
		commandBuffer.begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit );
		commandBuffer.copyBuffer( src.getBuffer().getBuffer()
			, getBuffer().getBuffer()
			, size
			, srcOffset );
		commandBuffer.end();
	}

	void GpuBuffer::upload( renderer::StagingBuffer & stagingBuffer
		, renderer::CommandBuffer const & commandBuffer
		, uint32_t offset
		, uint32_t count
		, uint8_t const * buffer )const
	{
		stagingBuffer.uploadBufferData( commandBuffer
			, buffer
			, count
			, offset
			, getBuffer() );
	}

	void GpuBuffer::download( renderer::StagingBuffer & stagingBuffer
		, renderer::CommandBuffer const & commandBuffer
		, uint32_t offset
		, uint32_t count
		, uint8_t * buffer )const
	{
		stagingBuffer.downloadBufferData( commandBuffer
			, buffer
			, count
			, offset
			, getBuffer() );
	}

	void GpuBuffer::doInitialiseStorage( renderer::Device const & device
		, uint32_t size
		, renderer::BufferTargets targets
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		m_buffer = renderer::makeBuffer< uint8_t >( device
			, size
			, targets
			, memoryFlags );
	}
}
