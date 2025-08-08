#include "Castor3D/Buffer/PoolUniformBuffer.hpp"

#include "Castor3D/Render/Buffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/ResourceHandler.hpp>

CU_ImplementSmartPtr( c3d, PoolUniformBuffer )

namespace c3d
{
	PoolUniformBuffer::PoolUniformBuffer( RenderSystem const & renderSystem
		, crg::ResourcesCache & resources
		, BufferUsageFlags usage
		, MemoryPropertyFlags flags
		, String debugName
		, ashes::QueueShare sharingMode )
		: m_renderSystem{ renderSystem }
		, m_resources{ resources }
		, m_usage{ usage | BufferUsageFlags::eUniformBuffer }
		, m_flags{ flags | MemoryPropertyFlags::eHostVisible }
		, m_sharingMode{ c3d::move( sharingMode ) }
		, m_debugName{ c3d::move( debugName ) }
		, m_elemSize{ m_renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment ) }
	{
		initialise( m_renderSystem.getRenderDevice() );
	}

	uint32_t PoolUniformBuffer::initialise( RenderDevice const & device )
	{
		if ( m_buffer )
			m_buffer->destroy();
		m_buffer.reset();
		auto maxSize = std::min( 65536u, m_renderSystem.getValue( GpuMax::eUniformBufferSize ) );
		auto elemCount = uint32_t( maxSize / m_elemSize );
		m_buffer = makeBufferBase( device, m_resources
			, elemCount * m_elemSize
			, m_usage | BufferUsageFlags::eTransferDst
			, m_flags
			, toUtf8( m_debugName + cuT( "Ubo" ) ) );
		m_buffer->create();
		m_data = makeArrayView( m_buffer->lock(), m_buffer->getSize() );
		return uint32_t( m_buffer->getSize() );
	}

	void PoolUniformBuffer::cleanup()noexcept
	{
		m_data = {};

		if ( m_buffer )
		{
			m_buffer->unlock();
			m_buffer->destroy();
			m_buffer.reset();
		}

	}

	void PoolUniformBuffer::flush()const
	{
		if ( m_buffer )
		{
			m_buffer->flush();
		}
	}

	DeviceSize PoolUniformBuffer::getAvailable()const noexcept
	{
		return hasAllocated()
			? m_buffer->getSize() - ( m_allocated.rbegin()->first.offset + m_allocated.rbegin()->first.size )
			: m_buffer->getSize();
	}

	Vector< Pair< MemChunk, String > > PoolUniformBuffer::listAllocations()const
	{
		return { m_allocated.begin(), m_allocated.end() };
	}

	bool PoolUniformBuffer::hasAvailable( DeviceSize size )const noexcept
	{
		return !hasAllocated()
			|| m_buffer->getSize() > ( m_allocated.rbegin()->first.offset + m_allocated.rbegin()->first.size + getAlignedSize( uint32_t( size ) ) );
	}

	bool PoolUniformBuffer::hasAllocated()const noexcept
	{
		return !m_allocated.empty();
	}

	MemChunk PoolUniformBuffer::allocate( DeviceSize size )
	{
		CU_Require( hasAvailable( size ) );
		auto elemSize = m_renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment );
		auto offset = m_allocated.empty()
			? 0u
			: m_allocated.rbegin()->first.offset + m_allocated.rbegin()->first.size;
		auto realSize = getAlignedSize( uint32_t( size ) );
		auto bufferViewId = m_resources.getHandler().createViewId( crg::BufferViewData{ "Chunk", m_buffer->bufferId, { offset, realSize } } );
#if !defined( NDEBUG )
		String stackTrace;
		StringStream stream = makeStringStream();
		stream << debug::Backtrace{ 20, 4 };
		stackTrace = stream.str();
		m_allocated.try_emplace( MemChunk{ offset, realSize, size, bufferViewId }, stackTrace );
#else
		m_allocated.try_emplace( MemChunk{ offset, realSize, size, bufferViewId } );
#endif
		return { offset / elemSize, realSize / elemSize, size, bufferViewId };
	}

	void PoolUniformBuffer::deallocate( DeviceSize offset )noexcept
	{
		auto elemSize = m_renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment );

		if ( auto it = m_allocated.find( { offset * elemSize, 0u, 0u, crg::BufferViewId{} } );
			it != m_allocated.end() )
		{
			m_allocated.erase( it );
		}
	}
}
