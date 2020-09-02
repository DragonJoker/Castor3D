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
	namespace  details
	{
		inline void copyBuffer( ashes::CommandBuffer const & commandBuffer
			, ashes::BufferBase const & src
			, ashes::BufferBase const & dst
			, VkDeviceSize offset
			, VkDeviceSize size
			, VkPipelineStageFlags flags )
		{
			auto srcSrcStage = src.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( srcSrcStage
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, src.makeTransferSource() );
			auto dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( dstSrcStage
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, dst.makeTransferDestination() );
			commandBuffer.copyBuffer( src
				, dst
				, uint32_t( size )
				, uint32_t( offset ) );
			srcSrcStage = src.getCompatibleStageFlags();
			dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( srcSrcStage
				, VK_PIPELINE_STAGE_HOST_BIT
				, src.makeHostWrite() );
			commandBuffer.memoryBarrier( dstSrcStage
				, flags
				, dst.makeUniformBufferInput() );
		}
	}

	template< typename DataT >
	UniformBufferPoolT< DataT >::UniformBufferPoolT( RenderSystem & renderSystem
		, castor::String debugName )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, m_debugName{ std::move( debugName ) }
	{
	}

	template< typename DataT >
	UniformBufferPoolT< DataT >::~UniformBufferPoolT()
	{
	}

	template< typename DataT >
	void UniformBufferPoolT< DataT >::cleanup()
	{
		m_buffers.clear();

		if ( m_stagingBuffer )
		{
			m_stagingBuffer->getBuffer().unlock();
			m_stagingBuffer.reset();
		}
	}

	template< typename DataT >
	void UniformBufferPoolT< DataT >::upload( ashes::CommandBuffer const & commandBuffer )const
	{
		if ( m_stagingBuffer )
		{
			m_stagingBuffer->getBuffer().flush( 0u, m_currentUboIndex * m_maxUboSize );

			for ( auto & bufferIt : m_buffers )
			{
				for ( auto & buffer : bufferIt.second )
				{
					details::copyBuffer( commandBuffer
						, m_stagingBuffer->getBuffer()
						, buffer.buffer->getBuffer().getBuffer()
						, buffer.index * m_maxUboElemCount
						, m_maxUboSize
						, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
				}
			}
		}
	}

	template< typename DataT >
	UniformBufferOffsetT< DataT > UniformBufferPoolT< DataT >::getBuffer( VkMemoryPropertyFlags flags )
	{
		UniformBufferOffsetT< DataT > result;

		auto key = uint32_t( flags );
		auto it = m_buffers.emplace( key, BufferArray{} ).first;
		auto itB = doFindBuffer( it->second );
		auto & renderSystem = *getRenderSystem();
		auto & device = *renderSystem.getMainRenderDevice();

		if ( itB == it->second.end() )
		{
			assert( m_currentUboIndex < m_maxPoolUboCount - 1u );
			ashes::QueueShare sharingMode
			{
				{
					device.getGraphicsQueueFamilyIndex(),
					device.getComputeQueueFamilyIndex(),
					device.getTransferQueueFamilyIndex(),
				}
			};

			if ( !m_maxUboElemCount )
			{
				auto & properties = renderSystem.getProperties();
				auto maxSize = std::min( 65536u, properties.limits.maxUniformBufferRange );
				auto elementSize = ashes::getAlignedSize( sizeof( DataT )
					, properties.limits.minUniformBufferOffsetAlignment );
				m_maxUboElemCount = uint32_t( std::floor( float( maxSize ) / elementSize ) );
				m_maxUboSize = uint32_t( m_maxUboElemCount * elementSize );
				m_stagingBuffer = std::make_unique< ashes::StagingBuffer >( *device
					, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
					, m_maxUboSize * m_maxPoolUboCount
					, sharingMode );
				m_stagingData = reinterpret_cast< uint8_t * >( m_stagingBuffer->getBuffer().lock( 0u
					, m_maxUboSize * m_maxPoolUboCount
					, 0u ) );
				assert( m_stagingData );
			}

			auto index = m_maxUboElemCount * m_currentUboIndex;
			auto buffer = makePoolUniformBuffer< DataT >( renderSystem
				, castor::makeArrayView( m_stagingData + index
					, m_stagingData + index + m_maxUboSize )
				, m_maxUboElemCount
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, flags
				, m_debugName );
			it->second.push_back( { m_currentUboIndex, std::move( buffer ) } );
			++m_currentUboIndex;
			itB = std::next( it->second.begin(), it->second.size() - 1 );
			m_maxUboSize = itB->buffer->initialise( sharingMode );
		}

		result.buffer = itB->buffer.get();
		result.offset = itB->buffer->allocate();
		result.flags = flags;
		return result;
	}

	template< typename DataT >
	void UniformBufferPoolT< DataT >::putBuffer( UniformBufferOffsetT< DataT > const & bufferOffset )
	{
		auto key = uint32_t( bufferOffset.flags );
		auto it = m_buffers.find( key );
		CU_Require( it != m_buffers.end() );
		auto itB = std::find_if( it->second.begin()
			, it->second.end()
			, [&bufferOffset]( Buffer const & lookup )
			{
				return lookup.buffer.get() == bufferOffset.buffer;
			} );
		CU_Require( itB != it->second.end() );
		itB->buffer->deallocate( bufferOffset.offset );
	}

	template< typename DataT >
	uint32_t UniformBufferPoolT< DataT >::getBufferCount()const
	{
		if ( !m_stagingBuffer )
		{
			return 0u;
		}

		uint32_t result = 0u;

		for ( auto & bufferIt : m_buffers )
		{
			result += uint32_t( bufferIt.second.size() );
		}

		return result;
	}

	template< typename DataT >
	typename UniformBufferPoolT< DataT >::BufferArray::iterator UniformBufferPoolT< DataT >::doFindBuffer( typename UniformBufferPoolT< DataT >::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end() && !it->buffer->hasAvailable() )
		{
			++it;
		}

		return it;
	}
}
