#include "Castor3D/Buffer/GpuBufferPool.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferBuddyAllocator.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/FramePass.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementSmartPtr( c3d, GpuBufferPool )

namespace c3d
{
	//*********************************************************************************************

	GpuBufferPool::GpuBufferPool( RenderDevice const & device
		, crg::ResourcesCache & resources
		, String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_resources{ resources }
		, m_debugName{ c3d::move( debugName ) }
		, m_minBlockSize{ uint32_t( device.renderSystem.getProperties().limits.minMemoryMapAlignment ) }
	{
	}

	void GpuBufferPool::upload( UploadData & uploader )const
	{
		for ( auto const & [_, buffers] : m_buffers )
		{
			for ( auto const & buffer : buffers )
			{
				buffer->upload( uploader );
			}
		}
	}

	AllocationStats GpuBufferPool::getAllocationStats()const noexcept
	{
		AllocationStats result{};

		for ( auto const & [_, buffers] : m_buffers )
		{
			for ( auto const & buffer : buffers )
			{
				result.total += buffer->getBuffer().getSize();
				result.available += buffer->getAvailable();
			}
		}

		return result;
	}

	GpuBufferBase & GpuBufferPool::doGetBuffer( VkDeviceSize size
		, BufferUsageFlags target
		, MemoryPropertyFlags memory
		, MemChunk & chunk )
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );

		if ( it == m_buffers.end() )
		{
			it = m_buffers.try_emplace( key ).first;
		}

		auto itB = doFindBuffer( size, it->second );

		if ( itB == it->second.end() )
		{
			VkDeviceSize level = 21u;
			VkDeviceSize maxSize = VkDeviceSize( 1ULL << level ) * m_minBlockSize;

			while ( size > maxSize && level <= 24 )
			{
				++level;
				maxSize = VkDeviceSize( 1ULL << level ) * m_minBlockSize;
			}

			CU_Require( maxSize < std::numeric_limits< uint32_t >::max() );
			CU_Require( maxSize >= size );

			auto buffer = makeRawUnique< GpuBuddyBuffer >( *getRenderSystem()
				, m_resources
				, target
				, memory
				, m_debugName
				, ashes::QueueShare{}
				, GpuBufferBuddyAllocator{ uint32_t( level ), m_minBlockSize } );
			it->second.emplace_back( c3d::move( buffer ) );
			itB = std::next( it->second.begin()
				, ptrdiff_t( it->second.size() - 1u ) );
		}

		chunk = ( *itB )->allocate( size );
		return *( *itB ).get();
	}

	void GpuBufferPool::doPutBuffer( GpuBufferBase const & buffer
		, BufferUsageFlags target
		, MemoryPropertyFlags memory
		, MemChunk const & chunk )noexcept
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );
		CU_Require( it != m_buffers.end() );
		if ( it != m_buffers.end() )
		{
			auto itB = std::find_if( it->second.begin(), it->second.end()
				, [&buffer]( RawUniquePtr< GpuBuddyBuffer > const & lookup )
				{
						return &lookup->getBuffer().getBuffer() == &buffer.getBuffer().getBuffer();
				} );
			CU_Require( itB != it->second.end() );
			if ( itB != it->second.end() )
			{
				( *itB )->deallocate( chunk );
			}
		}
	}

	GpuBufferPool::BufferArray::iterator GpuBufferPool::doFindBuffer( VkDeviceSize size
		, GpuBufferPool::BufferArray & array )const
	{
		auto it = array.begin();

		while ( it != array.end() && !( *it )->hasAvailable( size ) )
		{
			++it;
		}

		return it;
	}

	uint32_t GpuBufferPool::doMakeKey( BufferUsageFlags target
		, MemoryPropertyFlags flags )const noexcept
	{
		return ( uint32_t( target ) << 0u )
			| ( uint32_t( flags ) << 16u );
	}

	//*********************************************************************************************
}
