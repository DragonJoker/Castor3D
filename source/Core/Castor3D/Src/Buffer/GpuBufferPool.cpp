#include "GpuBufferPool.hpp"

#include "Render/RenderSystem.hpp"

#include <Buffer/Buffer.hpp>
#include <Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		uint32_t doMakeKey( ashes::BufferTarget target
			, ashes::MemoryPropertyFlags flags )
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

	GpuBufferOffset GpuBufferPool::getGpuBuffer( ashes::BufferTarget target
		, uint32_t size
		, ashes::MemoryPropertyFlags flags )
	{
		GpuBufferOffset result;

		if ( target != ashes::BufferTarget::eIndexBuffer
			&& target != ashes::BufferTarget::eVertexBuffer )
		{
			std::unique_ptr< GpuBuffer > buffer = std::make_unique< GpuBuffer >();
			buffer->doInitialiseStorage( getCurrentDevice( *getRenderSystem() )
				, size
				, target | ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
			m_nonSharedBuffers.emplace_back( std::move( buffer ) );
			result.buffer = &m_nonSharedBuffers.back()->getBuffer().getBuffer();
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

			if ( itB == it->second.end() )
			{
				uint32_t level = 20u;
				uint64_t maxSize = ( 1u << level ) * 96;

				while ( size > maxSize && level <= 24 )
				{
					++level;
					maxSize = ( 1u << level ) * 96;
				}

				CU_Require( maxSize < std::numeric_limits< uint32_t >::max() );
				CU_Require( maxSize >= size );

				std::unique_ptr< GpuBuffer > buffer = std::make_unique< GpuBuffer >();
				buffer->initialiseStorage( getCurrentDevice( *getRenderSystem() )
					, level
					, 96u
					, target
					, flags );
				it->second.emplace_back( std::move( buffer ) );
				result.buffer = &buffer->getBuffer().getBuffer();
				result.offset = buffer->allocate( size );
			}
			else
			{
				result.buffer = &( *itB )->getBuffer().getBuffer();
				result.offset = ( *itB )->allocate( size );
			}

			result.flags = flags;
		}

		return result;
	}

	void GpuBufferPool::putGpuBuffer( ashes::BufferTarget target
		, GpuBufferOffset const & bufferOffset )
	{
		if ( target != ashes::BufferTarget::eIndexBuffer
			&& target != ashes::BufferTarget::eVertexBuffer )
		{
			auto it = std::find_if( m_nonSharedBuffers.begin()
				, m_nonSharedBuffers.end()
				, [&bufferOffset]( std::unique_ptr< GpuBuffer > const & lookup )
				{
					return &lookup->getBuffer().getBuffer() == bufferOffset.buffer;
				} );
			CU_Require( it != m_nonSharedBuffers.end() );
			it->reset();
		}
		else
		{
			auto key = doMakeKey( target, bufferOffset.flags );
			auto it = m_buffers.find( key );
			CU_Require( it != m_buffers.end() );
			auto itB = std::find_if( it->second.begin()
				, it->second.end()
				, [&bufferOffset]( std::unique_ptr< GpuBuffer > const & lookup )
				{
					return &lookup->getBuffer().getBuffer() == bufferOffset.buffer;
				} );
			CU_Require( itB != it->second.end() );
			( *itB )->deallocate( bufferOffset.offset );
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
