#include "Castor3D/Buffer/GpuBuffer.hpp"

#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>

using namespace castor;

namespace castor3d
{
	GpuBufferBase::GpuBufferBase( RenderSystem const & renderSystem
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, castor::String debugName
		, ashes::QueueShare sharingMode
		, VkDeviceSize allocatedSize )
		: m_renderSystem{ renderSystem }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ std::move( sharingMode ) }
		, m_allocatedSize{ allocatedSize }
		, m_debugName{ std::move( debugName ) }
	{
		initialise( m_renderSystem.getRenderDevice() );
	}

	uint32_t GpuBufferBase::initialise( RenderDevice const & device )
	{
		m_buffer = makeBuffer< uint8_t >( device
			, uint32_t( m_allocatedSize )
			, m_usage
			, m_memoryFlags
			, m_debugName
			, m_sharingMode );
		return uint32_t( m_buffer->getBuffer().getSize() );
	}

	void GpuBufferBase::cleanup( RenderDevice const & device )
	{
		m_buffer.reset();
	}

	uint8_t * GpuBufferBase::lock( MemChunk const & chunk )const
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

	void GpuBufferBase::flush( MemChunk const & chunk )const
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

	void GpuBufferBase::invalidate( MemChunk const & chunk )const
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

	void GpuBufferBase::unlock()const
	{
		m_buffer->getBuffer().unlock();
	}

	void GpuBufferBase::copy( ashes::CommandBuffer const & commandBuffer
		, GpuBufferBase const & src
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

	void GpuBufferBase::upload( ashes::StagingBuffer & stagingBuffer
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

	void GpuBufferBase::upload( ashes::StagingBuffer & stagingBuffer
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

	void GpuBufferBase::download( ashes::StagingBuffer & stagingBuffer
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
