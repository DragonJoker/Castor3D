#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <Ashes/Command/CommandBuffer.hpp>
#include <Ashes/Core/Device.hpp>
#include <Ashes/Sync/Fence.hpp>

namespace castor3d
{
	template< typename T >
	UniformBufferPool< T >::UniformBufferPool( RenderSystem & renderSystem
		, castor::String debugName )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, m_debugName{ std::move( debugName ) }
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
		m_uploadCommandBuffer.reset();
		m_stagingBuffer.reset();
	}

	template< typename T >
	void UniformBufferPool< T >::upload( RenderPassTimer & timer, uint32_t index )const
	{
		for ( auto & bufferIt : m_buffers )
		{
			for ( auto & buffer : bufferIt.second )
			{
				buffer->upload( *m_stagingBuffer
					, *m_uploadCommandBuffer
					, 0u
					, ashes::PipelineStageFlag::eVertexShader
					, timer
					, index );
				timer.notifyPassRender( index );
				m_uploadFence->reset();

				getCurrentDevice( *getRenderSystem() ).getGraphicsQueue().submit( *m_uploadCommandBuffer
					, m_uploadFence.get() );
				m_uploadFence->wait( ashes::FenceTimeout );

				++index;
			}
		}
	}

	template< typename T >
	UniformBufferOffset< T > UniformBufferPool< T >::getBuffer( ashes::MemoryPropertyFlags flags )
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
			if ( !m_maxCount )
			{
				uint32_t maxSize = getRenderSystem()->getProperties().limits.maxUniformBufferRange;
				uint32_t minOffset = uint32_t( getRenderSystem()->getProperties().limits.minUniformBufferOffsetAlignment );
				uint32_t elementSize = 0u;
				uint32_t size = uint32_t( sizeof( T ) );

				while ( size > minOffset )
				{
					size -= minOffset;
					elementSize += minOffset;
				}

				elementSize += minOffset;
				m_maxCount = uint32_t( std::floor( float( maxSize ) / elementSize ) );
				m_maxSize = uint32_t( m_maxCount * elementSize );

				getRenderSystem()->getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
					, [this]()
					{
						auto & device = getCurrentDevice( *getRenderSystem() );
						m_uploadCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
						m_uploadFence = device.createFence( ashes::FenceCreateFlag::eSignaled );
						m_stagingBuffer = std::make_unique< ashes::StagingBuffer >( device
							, ashes::BufferTarget::eTransferSrc
							, m_maxSize );
					} ) );
			}

			auto buffer = std::make_unique< UniformBuffer< T > >( *getRenderSystem()
				, m_maxCount
				, flags
				, m_debugName );
			it->second.emplace_back( std::move( buffer ) );
			itB = it->second.begin() + it->second.size() - 1;
			auto & ubuffer = *it->second.back();

			getRenderSystem()->getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
				, [&ubuffer]()
				{
					ubuffer.initialise();
				} ) );
		}

		result.buffer = itB->get();
		result.offset = ( *itB )->allocate();
		result.flags = flags;
		return result;
	}

	template< typename T >
	void UniformBufferPool< T >::putBuffer( UniformBufferOffset< T > const & bufferOffset )
	{
		auto key = uint32_t( bufferOffset.flags );
		auto it = m_buffers.find( key );
		CU_Require( it != m_buffers.end() );
		auto itB = std::find_if( it->second.begin()
			, it->second.end()
			, [&bufferOffset]( std::unique_ptr< UniformBuffer< T > > const & lookup )
			{
				return lookup.get() == bufferOffset.buffer;
			} );
		CU_Require( itB != it->second.end() );
		( *itB )->deallocate( bufferOffset.offset );
	}

	template< typename T >
	uint32_t UniformBufferPool< T >::getBufferCount()const
	{
		uint32_t result = 0u;

		for ( auto & bufferIt : m_buffers )
		{
			result += uint32_t( bufferIt.second.size() );
		}

		return result;
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
