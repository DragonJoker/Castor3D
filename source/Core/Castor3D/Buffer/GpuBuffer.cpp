#include "Castor3D/Buffer/GpuBuffer.hpp"

#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>

using namespace castor;

namespace castor3d
{
	GpuBuffer::GpuBuffer( RenderSystem const & renderSystem
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, castor::String debugName
		, ashes::QueueShare sharingMode
		, uint32_t numLevels
		, uint32_t minBlockSize )
		: m_renderSystem{ renderSystem }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ std::move( sharingMode ) }
		, m_allocator{ numLevels, minBlockSize }
		, m_debugName{ std::move( debugName ) }
		, m_align{ minBlockSize }
	{
		initialise( m_renderSystem.getRenderDevice() );
	}

	uint32_t GpuBuffer::initialise( RenderDevice const & device )
	{
		CU_Require( ( m_align % device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) == 0u );
		m_buffer = makeBuffer< uint8_t >( device
			, uint32_t( m_allocator.getSize() )
			, m_usage
			, m_memoryFlags
			, m_debugName
			, m_sharingMode );
		return uint32_t( m_buffer->getBuffer().getSize() );
	}

	void GpuBuffer::cleanup( RenderDevice const & device )
	{
		m_buffer.reset();
	}

	bool GpuBuffer::hasAvailable( VkDeviceSize size )const
	{
		size = ashes::getAlignedSize( size, m_align );
		return m_allocator.hasAvailable( size );
	}

	MemChunk GpuBuffer::allocate( VkDeviceSize size )
	{
		size = ashes::getAlignedSize( size, m_align );
		return
		{
			size,
			m_allocator.allocate( size ),
		};
	}

	void GpuBuffer::deallocate( MemChunk const & mem )
	{
		m_allocator.deallocate( mem.offset );
	}

	uint8_t * GpuBuffer::lock( MemChunk const & chunk )const
	{
		auto size = chunk.size;
		auto offset = chunk.offset;

		if ( size > m_buffer->getBuffer().getSize() )
		{
			size = ~( 0ull );
			offset = 0u;
		}

		return m_buffer->getBuffer().lock( offset, size, 0u );
	}

	void GpuBuffer::flush( MemChunk const & chunk )const
	{
		auto size = chunk.size;
		auto offset = chunk.offset;

		if ( size > m_buffer->getBuffer().getSize() )
		{
			size = ~( 0ull );
			offset = 0u;
		}

		return m_buffer->getBuffer().flush( offset, size );
	}

	void GpuBuffer::invalidate( MemChunk const & chunk )const
	{
		auto size = chunk.size;
		auto offset = chunk.offset;

		if ( size > m_buffer->getBuffer().getSize() )
		{
			size = ~( 0ull );
			offset = 0u;
		}

		return m_buffer->getBuffer().invalidate( offset, size );
	}

	void GpuBuffer::unlock()const
	{
		m_buffer->getBuffer().unlock();
	}

	void GpuBuffer::copy( ashes::CommandBuffer const & commandBuffer
		, GpuBuffer const & src
		, MemChunk const & srcChunk
		, VkDeviceSize dstOffset )const
	{
		commandBuffer.copyBuffer( VkBufferCopy
			{
				srcChunk.offset,
				dstOffset,
				srcChunk.size,
			}
			, src.getBuffer().getBuffer()
			, getBuffer().getBuffer() );
	}

	void GpuBuffer::upload( ashes::StagingBuffer & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, MemChunk const & chunk
		, uint8_t const * buffer )const
	{
		stagingBuffer.uploadBufferData( commandBuffer
			, buffer
			, uint32_t( chunk.size )
			, uint32_t( chunk.offset )
			, getBuffer() );
	}

	void GpuBuffer::upload( ashes::StagingBuffer & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, MemChunk const & chunk
		, uint8_t const * buffer )const
	{
		stagingBuffer.uploadBufferData( queue
			, commandPool
			, buffer
			, uint32_t( chunk.size )
			, uint32_t( chunk.offset )
			, getBuffer() );
	}

	void GpuBuffer::download( ashes::StagingBuffer & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, MemChunk const & chunk
		, uint8_t * buffer )const
	{
		stagingBuffer.downloadBufferData( queue
			, commandPool
			, buffer
			, uint32_t( chunk.size )
			, uint32_t( chunk.offset )
			, getBuffer() );
	}
}
