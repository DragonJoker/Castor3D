#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	template< typename DataT >
	UniformBufferOffsetT< DataT > UniformBufferPool::getBuffer( VkMemoryPropertyFlags flags )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		UniformBufferOffsetT< DataT > result;

		auto & renderSystem = *getRenderSystem();
		auto alignment = renderSystem.getValue( GpuMin::eUniformBufferOffsetAlignment );
		auto key = uint32_t( flags );
		auto it = m_buffers.emplace( key, BufferArray{} ).first;
		auto itB = doFindBuffer( it->second, ashes::getAlignedSize( sizeof( DataT ), alignment ) );

		if ( itB == it->second.end() )
		{
			CU_Require( m_currentUboIndex < m_maxPoolUboCount - 1u );
			itB = doCreatePoolBuffer( flags, it->second );
			CU_Require( itB != it->second.end() );
		}

		result.setPool( *itB->buffer );
		auto chunk = itB->buffer->allocate( sizeof( DataT ) );
		result.offset = uint32_t( chunk.offset );
		result.range = uint32_t( chunk.size );
		result.flags = flags;
		return result;
	}

	template< typename DataT >
	void UniformBufferPool::putBuffer( UniformBufferOffsetT< DataT > const & bufferOffset )
	{
		if ( bufferOffset )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			auto key = uint32_t( bufferOffset.flags );
			auto it = m_buffers.find( key );
			CU_Require( it != m_buffers.end() );
			auto itB = std::find_if( it->second.begin()
				, it->second.end()
				, [&bufferOffset]( Buffer const & lookup )
				{
					return &lookup.buffer->getBuffer() == &bufferOffset.getBuffer();
				} );
			CU_Require( itB != it->second.end() );
			itB->buffer->deallocate( bufferOffset.offset );
		}
	}
}
