#include "GpuBufferPool.hpp"

#include "Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		uint32_t doMakeKey( BufferType type
			, BufferAccessType accessType
			, BufferAccessNature accessNature )
		{
			return ( uint32_t( type ) << 0u )
				| ( uint32_t( accessType ) << 8u )
				| ( uint32_t( accessNature ) << 8u );
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
		for ( auto & buffers : m_buffers )
		{
			for ( auto & buffer : buffers.second )
			{
				buffer->destroy();
			}
		}

		m_buffers.clear();
	}

	GpuBufferOffset GpuBufferPool::getGpuBuffer( BufferType type
		, uint32_t size
		, BufferAccessType accessType
		, BufferAccessNature accessNature )
	{
		GpuBufferOffset result;

		if ( type != BufferType::eArray
			&& type != BufferType::eElementArray )
		{
			result.buffer = getRenderSystem()->doCreateBuffer( type );
			result.buffer->create();
			result.buffer->doInitialiseStorage( size
				, accessType
				, accessNature );
			result.offset = 0u;
		}
		else
		{
			auto key = doMakeKey( type, accessType, accessNature );
			auto it = m_buffers.find( key );

			if ( it == m_buffers.end() )
			{
				it = m_buffers.emplace( key, BufferArray{} ).first;
			}

			auto itB = doFindBuffer( size, it->second );
			GpuBufferSPtr buffer;

			if ( itB == it->second.end() )
			{
				buffer = getRenderSystem()->doCreateBuffer( type );
				buffer->create();
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
					, accessType
					, accessNature );
				it->second.push_back( buffer );
			}
			else
			{
				buffer = *itB;
			}

			result.buffer = buffer;
			result.offset = buffer->allocate( size );
		}

		return result;
	}

	void GpuBufferPool::putGpuBuffer( BufferType type
		, BufferAccessType accessType
		, BufferAccessNature accessNature
		, GpuBufferOffset const & bufferOffset )
	{
		if ( type != BufferType::eArray
			&& type != BufferType::eElementArray )
		{
			bufferOffset.buffer->destroy();
		}
		else
		{
			auto key = doMakeKey( type, accessType, accessNature );
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
