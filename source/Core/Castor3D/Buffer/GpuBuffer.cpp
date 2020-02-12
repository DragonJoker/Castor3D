#include "Castor3D/Buffer/GpuBuffer.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	GpuBuffer::GpuBuffer()
	{
	}

	void GpuBuffer::initialiseStorage( RenderDevice const & device
		, uint32_t level
		, uint32_t minBlockSize
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, ashes::QueueShare sharingMode )
	{
		m_device = &device;
		m_allocator = std::make_unique< GpuBufferBuddyAllocator >( level, minBlockSize );
		doInitialiseStorage( device
			, uint32_t( m_allocator->getSize() )
			, usage
			, memoryFlags
			, std::move( sharingMode ) );
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
		, VkMemoryMapFlags const & flags )const
	{
		auto size64 = ashes::getAlignedSize( size
			, uint32_t( m_device->properties.limits.nonCoherentAtomSize ) );

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
			, uint32_t( m_device->properties.limits.nonCoherentAtomSize ) );

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
			, uint32_t( m_device->properties.limits.nonCoherentAtomSize ) );

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
		commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
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

	void GpuBuffer::upload( ashes::StagingBuffer & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, uint32_t count
		, uint8_t const * buffer )const
	{
		stagingBuffer.uploadBufferData( queue
			, commandPool
			, buffer
			, count
			, offset
			, getBuffer() );
	}

	void GpuBuffer::download( ashes::StagingBuffer & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, uint32_t count
		, uint8_t * buffer )const
	{
		stagingBuffer.downloadBufferData( queue
			, commandPool
			, buffer
			, count
			, offset
			, getBuffer() );
	}

	void GpuBuffer::doInitialiseStorage( RenderDevice const & device
		, uint32_t size
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, ashes::QueueShare sharingMode )
	{
		m_buffer = makeBuffer< uint8_t >( device
			, size
			, usage
			, memoryFlags
			, "GpuBuffer"
			, std::move( sharingMode ) );
	}
}
