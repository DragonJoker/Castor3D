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

	void GpuBuffer::initialiseStorage( ashes::Device const & device
		, uint32_t level
		, uint32_t minBlockSize
		, ashes::BufferTargets targets
		, ashes::MemoryPropertyFlags memoryFlags )
	{
		m_device = &device;
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
		, ashes::MemoryMapFlags const & flags )const
	{
		auto size64 = ashes::getAlignedSize( size
			, m_device->getProperties().limits.nonCoherentAtomSize );

		if ( size64 > m_buffer->getBuffer().getSize() )
		{
			size64 = ~( 0ull );
		}

		return m_buffer->getBuffer().lock( offset, size64, flags );
	}

	void GpuBuffer::flush( uint32_t offset
		, uint32_t size )const
	{
		auto size64 = ashes::getAlignedSize( size
			, m_device->getProperties().limits.nonCoherentAtomSize );

		if ( size64 > m_buffer->getBuffer().getSize() )
		{
			size64 = ~( 0ull );
		}

		return m_buffer->getBuffer().flush( offset, size64 );
	}

	void GpuBuffer::invalidate( uint32_t offset
		, uint32_t size )const
	{
		auto size64 = ashes::getAlignedSize( size
			, m_device->getProperties().limits.nonCoherentAtomSize );

		if ( size64 > m_buffer->getBuffer().getSize() )
		{
			size64 = ~( 0ull );
		}

		return m_buffer->getBuffer().invalidate( offset, size64 );
	}

	void GpuBuffer::unlock()const
	{
		m_buffer->getBuffer().unlock();
	}

	void GpuBuffer::copy( ashes::CommandBuffer const & commandBuffer
		, GpuBuffer const & src
		, uint32_t srcOffset
		, uint32_t dstOffset
		, uint32_t size )const
	{
		commandBuffer.begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
		commandBuffer.copyBuffer( src.getBuffer().getBuffer()
			, getBuffer().getBuffer()
			, size
			, srcOffset );
		commandBuffer.end();
	}

	void GpuBuffer::upload( ashes::StagingBuffer & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
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

	void GpuBuffer::download( ashes::StagingBuffer & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
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

	void GpuBuffer::doInitialiseStorage( ashes::Device const & device
		, uint32_t size
		, ashes::BufferTargets targets
		, ashes::MemoryPropertyFlags memoryFlags )
	{
		m_buffer = ashes::makeBuffer< uint8_t >( device
			, size
			, targets
			, memoryFlags );
	}
}
