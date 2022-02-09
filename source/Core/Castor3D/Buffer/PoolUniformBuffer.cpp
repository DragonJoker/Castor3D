#include "Castor3D/Buffer/PoolUniformBuffer.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <algorithm>

namespace castor3d
{
	PoolUniformBuffer::PoolUniformBuffer( RenderSystem const & renderSystem
		, castor::ArrayView< uint8_t > data
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String debugName
		, ashes::QueueShare sharingMode )
		: m_renderSystem{ renderSystem }
		, m_usage{ usage }
		, m_flags{ flags }
		, m_sharingMode{ std::move( sharingMode ) }
		, m_debugName{ std::move( debugName ) }
		, m_data{ std::move( data ) }
	{
		initialise( m_renderSystem.getRenderDevice() );
	}

	uint32_t PoolUniformBuffer::initialise( RenderDevice const & device )
	{
		m_buffer.reset();
		auto elemSize = m_renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment );
		auto maxSize = std::min( 65536u, m_renderSystem.getValue( GpuMax::eUniformBufferSize ) );
		auto elemCount = uint32_t( std::floor( float( maxSize ) / float( elemSize ) ) );
		m_buffer = ashes::makeUniformBuffer( *device.device
			, m_debugName + "Ubo"
			, elemCount
			, elemSize
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, m_sharingMode );
		m_buffer->bindMemory( setupMemory( device, *m_buffer, m_flags, m_debugName + "Ubo" ) );
		return uint32_t( m_buffer->getBuffer().getSize() );
	}

	void PoolUniformBuffer::cleanup( RenderDevice const & device )
	{
		m_buffer.reset();
	}

	bool PoolUniformBuffer::hasAvailable( VkDeviceSize size )const
	{
		return !hasAllocated()
			|| m_buffer->getBuffer().getSize() > ( m_allocated.rbegin()->offset + m_allocated.rbegin()->size + getAlignedSize( uint32_t( size ) ) );
	}

	bool PoolUniformBuffer::hasAllocated()const
	{
		return !m_allocated.empty();
	}

	MemChunk PoolUniformBuffer::allocate( VkDeviceSize size )
	{
		CU_Require( hasAvailable( size ) );
		auto elemSize = m_renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment );
		auto offset = m_allocated.empty()
			? 0u
			: m_allocated.rbegin()->offset + m_allocated.rbegin()->size;
		auto realSize = getAlignedSize( uint32_t( size ) );
		m_allocated.insert( { offset, realSize, size } );
		return { offset / elemSize, realSize / elemSize, size };
	}

	void PoolUniformBuffer::deallocate( VkDeviceSize offset )
	{
		auto elemSize = m_renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment );
		auto it = m_allocated.find( { offset * elemSize, 0u, 0u } );

		if ( it != m_allocated.end() )
		{
			m_allocated.erase( it );
		}
	}
}
