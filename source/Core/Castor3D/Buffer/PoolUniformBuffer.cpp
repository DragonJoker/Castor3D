#include "Castor3D/Buffer/PoolUniformBuffer.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <algorithm>

CU_ImplementSmartPtr( castor3d, PoolUniformBuffer )

namespace castor3d
{
	PoolUniformBuffer::PoolUniformBuffer( RenderSystem const & renderSystem
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String debugName
		, ashes::QueueShare sharingMode )
		: m_renderSystem{ renderSystem }
		, m_usage{ usage }
		, m_flags{ flags | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT }
		, m_sharingMode{ castor::move( sharingMode ) }
		, m_debugName{ castor::move( debugName ) }
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
			, castor::toUtf8( m_debugName + cuT( "Ubo" ) )
			, elemCount
			, elemSize
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_sharingMode );
		m_buffer->bindMemory( setupMemory( device, *m_buffer, m_flags, m_debugName + cuT( "Ubo" ) ) );
		m_data = castor::makeArrayView( m_buffer->getBuffer().lock( 0u, ashes::WholeSize, 0u )
			, m_buffer->getBuffer().getSize() );
		return uint32_t( m_buffer->getBuffer().getSize() );
	}

	void PoolUniformBuffer::cleanup( RenderDevice const & )noexcept
	{
		m_data = {};

		if ( m_buffer )
		{
			m_buffer->getBuffer().unlock();
			m_buffer.reset();
		}

	}

	void PoolUniformBuffer::flush()
	{
		if ( m_buffer )
		{
			m_buffer->getBuffer().flush( 0u, ashes::WholeSize );
		}
	}

	bool PoolUniformBuffer::hasAvailable( VkDeviceSize size )const noexcept
	{
		return !hasAllocated()
			|| m_buffer->getBuffer().getSize() > ( m_allocated.rbegin()->offset + m_allocated.rbegin()->size + getAlignedSize( uint32_t( size ) ) );
	}

	bool PoolUniformBuffer::hasAllocated()const noexcept
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
		m_allocated.emplace( offset, realSize, size );
		return { offset / elemSize, realSize / elemSize, size };
	}

	void PoolUniformBuffer::deallocate( VkDeviceSize offset )noexcept
	{
		auto elemSize = m_renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment );

		if ( auto it = m_allocated.find( { offset * elemSize, 0u, 0u } );
			it != m_allocated.end() )
		{
			m_allocated.erase( it );
		}
	}
}
