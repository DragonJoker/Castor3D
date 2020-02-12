#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

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
		m_stagingBuffer.reset();
	}

	template< typename T >
	void UniformBufferPool< T >::upload( RenderPassTimer & timer, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( *this );

		for ( auto & bufferIt : m_buffers )
		{
			for ( auto & buffer : bufferIt.second )
			{
				buffer->upload( m_stagingBuffer->getBuffer()
					, *device.transferQueue
					, *device.transferCommandPool
					, 0u
					, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
					, timer
					, index );
				timer.notifyPassRender( index );
				++index;
			}
		}
	}

	template< typename T >
	UniformBufferOffset< T > UniformBufferPool< T >::getBuffer( VkMemoryPropertyFlags flags )
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
			auto & renderSystem = *getRenderSystem();

			if ( !m_maxCount )
			{
				auto & properties = renderSystem.getProperties();
				auto maxSize = std::min( 65536u, properties.limits.maxUniformBufferRange );
				auto elementSize = ashes::getAlignedSize( sizeof( T )
					, properties.limits.minUniformBufferOffsetAlignment );
				m_maxCount = uint32_t( std::floor( float( maxSize ) / elementSize ) );
				m_maxSize = uint32_t( m_maxCount * elementSize );

				renderSystem.getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
					, [this]()
					{
						auto & device = getCurrentRenderDevice( *this );
						m_stagingBuffer = std::make_unique< ashes::StagingBuffer >( *device
							, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
							, m_maxSize
							, ashes::QueueShare
							{
								{
									device.graphicsQueueFamilyIndex,
									device.computeQueueFamilyIndex,
									device.transferQueueFamilyIndex,
								}
							} );
					} ) );
			}

			ashes::QueueShare sharingMode;

			if ( renderSystem.hasMainDevice() )
			{
				auto & device = *renderSystem.getMainRenderDevice();
				sharingMode =
				{
					{
						device.graphicsQueueFamilyIndex,
						device.computeQueueFamilyIndex,
						device.transferQueueFamilyIndex,
					}
				};
			}

			auto buffer = makeUniformBuffer< T >( renderSystem
				, m_maxCount
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, flags
				, m_debugName );
			it->second.emplace_back( std::move( buffer ) );
			itB = it->second.begin() + it->second.size() - 1;
			auto & ubuffer = *it->second.back();

			renderSystem.getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
				, [&ubuffer, sharingMode, this]()
				{
					auto & device = getCurrentRenderDevice( *this );
					m_maxSize = ubuffer.initialise( 
						{
							{
								device.graphicsQueueFamilyIndex,
								device.computeQueueFamilyIndex,
								device.transferQueueFamilyIndex,
							}
						} );
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
			, [&bufferOffset]( UniformBufferUPtr< T > const & lookup )
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
