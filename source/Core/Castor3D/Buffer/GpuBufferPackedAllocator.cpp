#include "Castor3D/Buffer/GpuBufferPackedAllocator.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

CU_ImplementSmartPtr( castor3d, GpuPackedBuffer )
CU_ImplementSmartPtr( castor3d, GpuPackedBaseBuffer )

namespace castor3d
{
	static bool constexpr C3D_DebugGPUPackedAllocator = false;

	GpuBufferPackedAllocator::GpuBufferPackedAllocator( size_t size
		, size_t alignSize )
		: m_allocatedSize{ size }
		, m_alignSize{ alignSize }
	{
	}

	VkDeviceSize GpuBufferPackedAllocator::allocate( size_t size )
	{
		CU_Require( hasAvailable( size ) );
		MemChunk chunk{ 0u, size, size };
		size = size_t( ashes::getAlignedSize( size, m_alignSize ) );
		chunk.size = size;

		if ( auto it = std::find_if( m_deallocated.begin()
			, m_deallocated.end()
			, [size]( MemChunk const & lookup )
			{
				return lookup.size >= size;
			} );
			it != m_deallocated.end() )
		{
			chunk.offset = it->offset;

			if constexpr ( C3D_DebugGPUPackedAllocator )
			{
				if ( 1u < std::count_if( m_deallocated.begin()
					, m_deallocated.end()
					, [&chunk]( MemChunk const & lookup )
					{
						return lookup.offset == chunk.offset;
					} ) )
				{
					log::error << "Duplicate offset in deallocated chunks" << size << std::endl;
					CU_Failure( "Duplicate offset in deallocated chunks" );
				}
			}

			if ( it->size > size )
			{
				it->size -= size;
				it->askedSize -= size;
				it->offset += size;

				if constexpr ( C3D_DebugGPUPackedAllocator )
				{
					auto offset = it->offset;
					auto count = std::count_if( m_deallocated.begin()
						, m_deallocated.end()
						, [offset]( MemChunk const & lookup )
						{
								return lookup.offset == offset;
						} );

					if ( count > 1u )
					{
						log::error << "Duplicate offset in deallocated chunks" << size << std::endl;
						CU_Failure( "Duplicate offset in deallocated chunks" );
					}

					count = std::count_if( m_deallocated.begin()
						, m_deallocated.end()
						, [&chunk]( MemChunk const & lookup )
						{
								return lookup.offset == chunk.offset;
						} );

					if ( count > 0u )
					{
						log::error << "Duplicate offset in deallocated chunks" << size << std::endl;
						CU_Failure( "Duplicate offset in deallocated chunks" );
					}
				}
			}
			else
			{
				m_deallocated.erase( it );
			}
		}
		else if ( !m_allocated.empty() )
		{
			auto rit = m_allocated.rbegin();
			chunk.offset = rit->offset + rit->size;
			CU_Require( chunk.offset == ashes::getAlignedSize( chunk.offset, m_alignSize ) );
		}

		if ( chunk.offset + size > m_allocatedSize )
		{
			log::error << "Trying to allocate more than possible (" << m_allocatedSize
				<< "): offset = " << chunk.offset
				<< ", size = " << size << std::endl;
			CU_Failure( "Trying to allocate more than possible" );
		}

		if constexpr ( C3D_DebugGPUPackedAllocator )
		{
			if ( auto it = std::find_if( m_deallocated.begin()
				, m_deallocated.end()
				, [&chunk]( MemChunk const & lookup )
				{
					return lookup.offset == chunk.offset;
				} );
				it != m_deallocated.end() )
			{
				log::error << "Trying to allocate at a remaining deallocated offset (" << m_allocatedSize
					<< "): offset = " << chunk.offset
					<< ", size = " << size << std::endl;
				CU_Failure( "Trying to allocate at a remaining deallocated offset" );
			}

			if ( auto ait = m_allocated.find( chunk );
				ait != m_allocated.end() )
			{
				log::error << "Trying to allocate at an already allocated offset (" << m_allocatedSize
					<< "): offset = " << chunk.offset
					<< ", size = " << size << std::endl;
				CU_Failure( "Trying to allocate at an already allocated offset" );
			}
		}

		m_currentAllocated += chunk.size;
		m_allocated.insert( chunk );
		return chunk.offset;
	}

	void GpuBufferPackedAllocator::deallocate( VkDeviceSize pointer )
	{
		CU_Require( pointer < m_allocatedSize );

		if ( auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [pointer]( MemChunk const & lookup )
			{
				return lookup.offset == pointer;
			} );
			it != m_allocated.end() )
		{
			auto chunk = *it;
			m_allocated.erase( it );
			m_currentAllocated -= chunk.size;

			if ( m_currentAllocated == 0u )
			{
				m_deallocated.clear();
			}
			else
			{
				auto dit = std::find_if( m_deallocated.begin()
					, m_deallocated.end()
					, [&chunk]( MemChunk const & lookup )
					{
						return lookup.offset == chunk.offset + chunk.size
							|| chunk.offset == lookup.offset + lookup.size;
					} );

				if ( dit != m_deallocated.end() )
				{
					dit->offset = std::min( dit->offset, chunk.offset );
					dit->size += chunk.size;
					dit->askedSize += chunk.size;
				}
				else
				{
					dit = std::lower_bound( m_deallocated.begin()
						, m_deallocated.end()
						, chunk
						, MemChunkCompare{} );
					m_deallocated.emplace( dit, chunk );
				}
			}
		}
		else
		{
			log::error << "Trying to deallocate a memory chunk that doesn't belong to this buffer" << std::endl;
		}
	}
}
