#include "GpuBuffer.hpp"

#include "Render/RenderSystem.hpp"
#include "Render/Context.hpp"

#include <Buffer.hpp>
#include <Device.hpp>

using namespace castor;

namespace castor3d
{
	GpuBuffer::GpuBuffer( RenderSystem & renderSystem
		, renderer::BufferTarget target )
		: OwnedBy< RenderSystem >( renderSystem )
		, m_target{ target }
	{
	}

	void GpuBuffer::cleanupStorage()
	{
		REQUIRE( !m_allocator );
		m_storage.reset();
	}

	void GpuBuffer::initialiseStorage( uint32_t level
		, uint32_t minBlockSize
		, renderer::MemoryPropertyFlags flags )
	{
		m_allocator = std::make_unique< GpuBufferAllocator >( level, minBlockSize );
		m_storage = getRenderSystem()->getCurrentContext()->getDevice().createBuffer( uint32_t( m_allocator->getSize() )
			, m_target
			, flags );
		m_flags = flags;
	}

	void GpuBuffer::initialiseStorage( uint32_t size
		, renderer::MemoryPropertyFlags flags )
	{
		m_allocator.reset();
		m_storage = getRenderSystem()->getCurrentContext()->getDevice().createBuffer( size
			, m_target
			, flags );
		m_flags = flags;
	}

	bool GpuBuffer::hasAvailable( size_t size )const
	{
		REQUIRE( m_allocator );
		return m_allocator->hasAvailable( size );
	}

	uint32_t GpuBuffer::allocate( size_t size )
	{
		REQUIRE( m_allocator );
		return m_allocator->allocate( size );
	}

	void GpuBuffer::deallocate( uint32_t offset )
	{
		REQUIRE( m_allocator );
		m_allocator->deallocate( offset );
	}

	uint8_t * GpuBuffer::lock( uint32_t offset
		, uint32_t size
		, renderer::MemoryMapFlags const & flags )const
	{
		REQUIRE( m_storage );
		return m_storage->lock( offset
			, size
			, flags );
	}

	void GpuBuffer::unlock( uint32_t size, bool modified )const
	{
		REQUIRE( m_storage );
		m_storage->unlock( size, modified );
	}
}
