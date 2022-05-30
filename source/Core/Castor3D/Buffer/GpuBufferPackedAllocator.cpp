#include "Castor3D/Buffer/GpuBufferPackedAllocator.hpp"

namespace castor3d
{
	GpuBufferPackedAllocator::GpuBufferPackedAllocator( size_t size
		, size_t alignSize )
		: m_allocatedSize{ size }
		, m_alignSize{ alignSize }
	{
	}

	VkDeviceSize GpuBufferPackedAllocator::allocate( size_t size )
	{
		CU_Require( hasAvailable( size ) );
		size = ashes::getAlignedSize( size, m_alignSize );
		auto it = std::find_if( m_deallocated.begin()
			, m_deallocated.end()
			, [size]( MemChunk const & lookup )
			{
				return lookup.size >= size;
			} );
		MemChunk chunk{ 0u, size, size };

		if ( it != m_deallocated.end() )
		{
			if ( it->size > size )
			{
				it->size -= size;
				it->offset += size;
			}
			else
			{
				chunk.offset = it->offset;
				m_deallocated.erase( it );
			}
		}
		else if ( !m_allocated.empty() )
		{
			auto rit = m_allocated.rbegin();
			chunk.offset = rit->offset + rit->size;
		}

		m_currentAllocated += size;
		m_allocated.insert( chunk );
		return chunk.offset;
	}

	void GpuBufferPackedAllocator::deallocate( VkDeviceSize pointer )
	{
		CU_Require( pointer < m_allocatedSize );
		auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [pointer]( MemChunk const & lookup )
			{
				return lookup.offset == pointer;
			} );

		if ( it != m_allocated.end() )
		{
			m_currentAllocated -= it->size;
			m_deallocated.push_back( *it );
			m_allocated.erase( it );
		}
	}
}
