#include "GpuBufferPool.hpp"

#include "Render/RenderSystem.hpp"

#include <Buffer/Buffer.hpp>
#include <Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		uint32_t doMakeKey( renderer::BufferTarget target
			, renderer::MemoryPropertyFlags flags )
		{
			return ( uint32_t( target ) << 0u )
				| ( uint32_t( flags ) << 16u );
		}
	}

	GpuBufferPool::GpuBufferPool( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >{ renderSystem }
	{
	}

	GpuBufferPool::~GpuBufferPool()
	{
	}

	void GpuBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	GpuBufferOffset GpuBufferPool::getGpuBuffer( renderer::BufferTarget target
		, uint32_t size
		, renderer::MemoryPropertyFlags flags )
	{
		GpuBufferOffset result;

		if ( target != renderer::BufferTarget::eIndexBuffer
			&& target != renderer::BufferTarget::eVertexBuffer )
		{
			auto & device = *getRenderSystem()->getCurrentDevice();
			m_nonSharedBuffers.emplace_back( device.createBuffer( size
				, target | renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible ) );
			result.buffer = m_nonSharedBuffers.back().get();
			result.offset = 0u;
		}
		else
		{
			auto key = doMakeKey( target, flags );
			auto it = m_buffers.find( key );

			if ( it == m_buffers.end() )
			{
				it = m_buffers.emplace( key, BufferArray{} ).first;
			}

			auto itB = doFindBuffer( size, it->second );
			GpuBufferSPtr buffer;

			if ( itB == it->second.end() )
			{
				result.buffer = std::make_shared< GpuBuffer >( *getRenderSystem()
					, target );
				uint32_t level = 20u;
				uint64_t maxSize = ( 1u << level ) * 96;

				while ( size > maxSize && level <= 24 )
				{
					++level;
					maxSize = ( 1u << level ) * 96;
				}

				REQUIRE( maxSize < std::numeric_limits< uint32_t >::max() );
				REQUIRE( maxSize >= size );

				buffer->initialiseStorage( level
					, 96u
					, flags );
				it->second.push_back( buffer );
			}
			else
			{
				buffer = *itB;
			}

			result.buffer = buffer;
			result.offset = buffer->allocate( size );
			result.flags = flags;
		}

		return result;
	}

	void GpuBufferPool::putGpuBuffer( renderer::BufferTarget target
		, GpuBufferOffset const & bufferOffset )
	{
		if ( target != renderer::BufferTarget::eIndexBuffer
			&& target != renderer::BufferTarget::eVertexBuffer )
		{
			auto it = std::find( m_nonSharedBuffers.begin()
				, m_nonSharedBuffers.end()
				, bufferOffset.buffer );
			REQUIRE( it != m_nonSharedBuffers.end() );
			it->reset();
		}
		else
		{
			auto key = doMakeKey( target, bufferOffset.flags );
			auto it = m_buffers.find( key );
			REQUIRE( it != m_buffers.end() );
			auto itB = std::find_if( it->second.begin()
				, it->second.end()
				, [&bufferOffset]( GpuBufferSPtr const & lookup )
				{
					return lookup == bufferOffset.buffer;
				} );
			REQUIRE( itB != it->second.end() );
			auto buffer = *itB;
			buffer->deallocate( bufferOffset.offset );
		}
	}

	GpuBufferPool::BufferArray::iterator GpuBufferPool::doFindBuffer( uint32_t size
		, GpuBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end() && !( *it )->hasAvailable( size ) )
		{
			++it;
		}

		return it;
	}
}
