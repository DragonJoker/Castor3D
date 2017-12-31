#include "GpuBuffer.hpp"

using namespace castor;

namespace castor3d
{
	GpuBuffer::GpuBuffer( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >( renderSystem )
	{
	}

	GpuBuffer::~GpuBuffer()
	{
	}

	void GpuBuffer::initialiseStorage( uint32_t level
		, uint32_t minBlockSize
		, BufferAccessType type
		, BufferAccessNature nature )
	{
		m_allocator = std::make_unique< GpuBufferAllocator >( level, minBlockSize );
		doInitialiseStorage( uint32_t( m_allocator->getSize() )
			, type
			, nature );
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
}
