#include "Render/RenderSystem.hpp"

#include <Core/Device.hpp>

namespace castor3d
{
	template< typename T >
	UniformBufferPool< T >::UniformBufferPool( RenderSystem & renderSystem )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
	{
	}

	template< typename T >
	UniformBufferPool< T >::~UniformBufferPool()
	{
	}

	template< typename T >
	void UniformBufferPool< T >::cleanup()
	{
		m_buffers.clear();
	}

	template< typename T >
	UniformBufferOffset< T > UniformBufferPool< T >::getBuffer( renderer::MemoryPropertyFlags flags )
	{
		UniformBufferOffset< T > result;

		auto key = uint32_t( flags );
		auto it = m_buffers.find( key );

		if ( it == m_buffers.end() )
		{
			it = m_buffers.emplace( key, BufferArray{} ).first;
		}

		auto itB = doFindBuffer( it->second );

		if ( itB == it->second.end() )
		{
			uint64_t maxSize = getRenderSystem()->getCurrentDevice()->getProperties().limits.maxUniformBufferRange;
			auto buffer = std::make_unique< UniformBuffer< T > >( *getRenderSystem()->getCurrentDevice()
				, uint32_t( std::floor( float( maxSize ) / sizeof( T ) ) )
				, flags );
			it->second.emplace_back( std::move( buffer ) );
			itB = it->second.begin() + it->second.size() - 1;
		}

		result.buffer = itB->get();
		result.offset = ( *itB )->allocate();
		result.flags = flags;
		return result;
	}

	template< typename T >
	void UniformBufferPool< T >::putBuffer( UniformBufferOffset< T > const & bufferOffset )
	{
		auto key = uint32( bufferOffset.flags );
		auto it = m_buffers.find( key );
		REQUIRE( it != m_buffers.end() );
		auto itB = std::find_if( it->second.begin()
			, it->second.end()
			, [&bufferOffset]( std::unique_ptr< UniformBuffer< T > > const & lookup )
			{
				return &lookup.get() == &bufferOffset.buffer;
			} );
		REQUIRE( itB != it->second.end() );
		( *itB )->deallocate( bufferOffset.offset );
	}

	template< typename T >
	typename UniformBufferPool< T >::BufferArray::iterator UniformBufferPool< T >::doFindBuffer( typename UniformBufferPool< T >::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end() && !( *it )->hasAvailable() )
		{
			++it;
		}

		return it;
	}
}
