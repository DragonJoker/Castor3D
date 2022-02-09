#include "Castor3D/Buffer/GpuBufferPool.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

using namespace castor;

namespace castor3d
{
	GpuBufferPool::GpuBufferPool( RenderSystem & renderSystem
		, RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_device{ device }
		, m_debugName{ std::move( debugName ) }
		, m_minBlockSize{ uint32_t( renderSystem.getProperties().limits.minMemoryMapAlignment ) }
	{
	}

	void GpuBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	GpuBufferBase & GpuBufferPool::doGetBuffer( VkDeviceSize size
		, VkBufferUsageFlags target
		, VkMemoryPropertyFlags memory
		, MemChunk & chunk )
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );

		if ( it == m_buffers.end() )
		{
			it = m_buffers.emplace( key, BufferArray{} ).first;
		}

		auto itB = doFindBuffer( size, it->second );

		if ( itB == it->second.end() )
		{
			VkDeviceSize level = 21u;
			VkDeviceSize maxSize = ( 1u << level ) * m_minBlockSize;

			while ( size > maxSize && level <= 24 )
			{
				++level;
				maxSize = ( 1u << level ) * m_minBlockSize;
			}

			CU_Require( maxSize < std::numeric_limits< uint32_t >::max() );
			CU_Require( maxSize >= size );

			auto buffer = std::make_unique< GpuBuddyBuffer >( *getRenderSystem()
				, target
				, memory
				, m_debugName
				, ashes::QueueShare{}
				, GpuBufferBuddyAllocator{ uint32_t( level ), m_minBlockSize } );
			buffer->initialise( m_device );
			it->second.emplace_back( std::move( buffer ) );
			itB = std::next( it->second.begin()
				, ptrdiff_t( it->second.size() - 1u ) );
		}

		chunk = ( *itB )->allocate( size );
		return *( *itB ).get();
	}

	void GpuBufferPool::doPutBuffer( GpuBufferBase const & buffer
		, VkBufferUsageFlags target
		, VkMemoryPropertyFlags memory
		, MemChunk const & chunk )
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );
		CU_Require( it != m_buffers.end() );
		auto itB = std::find_if( it->second.begin()
			, it->second.end()
			, [&buffer]( std::unique_ptr< GpuBuddyBuffer > const & lookup )
			{
				return &lookup->getBuffer().getBuffer() == &buffer.getBuffer().getBuffer();
			} );
		CU_Require( itB != it->second.end() );
		( *itB )->deallocate( chunk );
	}

	GpuBufferPool::BufferArray::iterator GpuBufferPool::doFindBuffer( VkDeviceSize size
		, GpuBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end() && !( *it )->hasAvailable( size ) )
		{
			++it;
		}

		return it;
	}

	uint32_t GpuBufferPool::doMakeKey( VkBufferUsageFlags target
		, VkMemoryPropertyFlags flags )
	{
		return ( uint32_t( target ) << 0u )
			| ( flags << 16u );
	}
}
