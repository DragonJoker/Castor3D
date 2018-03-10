/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Buffer/Buffer.hpp"

namespace renderer
{
	BufferBase::BufferBase( Device const & device
		, uint32_t size
		, BufferTargets target )
		: m_device{ device }
		, m_size{ size }
	{
	}

	void BufferBase::bindMemory( DeviceMemoryPtr memory )
	{
		assert( !m_storage && "A resource can only be bound once to a device memory object." );
		m_storage = std::move( memory );
		doBindMemory();
	}

	uint8_t * BufferBase::lock( uint32_t offset
		, uint32_t size
		, MemoryMapFlags flags )const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		return m_storage->lock( offset, size, flags );
	}

	void BufferBase::invalidate( uint32_t offset
		, uint32_t size )const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		return m_storage->invalidate( offset, size );
	}

	void BufferBase::flush( uint32_t offset
		, uint32_t size )const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		return m_storage->flush( offset, size );
	}

	void BufferBase::unlock()const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		return m_storage->unlock();
	}
}
