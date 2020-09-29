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
		if ( m_renderSystem.hasCurrentRenderDevice() )
		{
			initialise( m_renderSystem.getCurrentRenderDevice() );
		}
	}

	uint32_t PoolUniformBuffer::initialise( RenderDevice const & device )
	{
		m_buffer.reset();
		auto & properties = m_renderSystem.getProperties();
		auto elemSize = properties.limits.minUniformBufferOffsetAlignment;
		auto maxSize = std::min( 65536u, properties.limits.maxUniformBufferRange );
		auto elemCount = uint32_t( std::floor( float( maxSize ) / elemSize ) );
		m_buffer = ashes::makeUniformBuffer( *device.device
			, m_debugName + "Ubo"
			, elemSize
			, elemCount
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
		return m_allocated.empty()
			|| m_buffer->getBuffer().getSize() >+ ( m_allocated.rbegin()->offset + m_allocated.rbegin()->size + getAlignedSize( uint32_t( size ) ) );
	}

	MemChunk PoolUniformBuffer::allocate( VkDeviceSize size )
	{
		CU_Require( hasAvailable( size ) );
		auto & properties = m_renderSystem.getProperties();
		auto elemSize = properties.limits.minUniformBufferOffsetAlignment;
		auto offset = m_allocated.empty()
			? 0u
			: m_allocated.rbegin()->offset + m_allocated.rbegin()->size;
		size = getAlignedSize( uint32_t( size ) );
		m_allocated.insert( { offset, size } );
		return { offset / elemSize, size / elemSize };
	}

	void PoolUniformBuffer::deallocate( VkDeviceSize offset )
	{
		auto & properties = m_renderSystem.getProperties();
		auto elemSize = properties.limits.minUniformBufferOffsetAlignment;
		auto it = m_allocated.find( { offset * elemSize, 0u } );

		if ( it != m_allocated.end() )
		{
			m_allocated.erase( it );
		}
	}
}
