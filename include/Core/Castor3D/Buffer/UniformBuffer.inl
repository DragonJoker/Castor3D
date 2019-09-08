#include "Castor3D/Render/RenderSystem.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

#include <algorithm>

namespace castor3d
{
	namespace
	{
		inline void copyBuffer( ashes::BufferBase const & src
			, ashes::BufferBase const & dst
			, ashes::BufferBase const * ubo
			, ashes::CommandBuffer const & commandBuffer
			, VkDeviceSize elemAlignedSize
			, VkDeviceSize count
			, VkDeviceSize offset
			, VkPipelineStageFlags flags
			, RenderPassTimer const & timer
			, uint32_t index )
		{
			commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			timer.beginPass( commandBuffer, index );
			commandBuffer.memoryBarrier( src.getCompatibleStageFlags()
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, src.makeTransferSource() );
			commandBuffer.memoryBarrier( dst.getCompatibleStageFlags()
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, dst.makeTransferDestination() );
			commandBuffer.copyBuffer( src
				, dst
				, uint32_t( elemAlignedSize * count )
				, uint32_t( elemAlignedSize * offset ) );

			if ( ubo == &src )
			{
				commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, flags
					, src.makeUniformBufferInput() );
			}
			else
			{
				commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, VK_PIPELINE_STAGE_HOST_BIT
					, src.makeHostWrite() );
			}

			if ( ubo == &dst )
			{
				commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, flags
					, dst.makeUniformBufferInput() );
			}
			else
			{
				commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, VK_PIPELINE_STAGE_HOST_BIT
					, dst.makeHostRead() );
			}

			timer.endPass( commandBuffer, index );
			commandBuffer.end();
		}
	}

	template< typename T >
	inline UniformBuffer< T >::UniformBuffer( RenderSystem const & renderSystem
		, uint32_t count
		, VkMemoryPropertyFlags flags
		, castor::String debugName )
		: m_renderSystem{ renderSystem }
		, m_flags{ flags }
		, m_count{ count }
	{
		for ( uint32_t i = 0; i < count; ++i )
		{
			m_available.insert( i );
		}
	}

	template< typename T >
	inline UniformBuffer< T >::~UniformBuffer()
	{
	}

	template< typename T >
	inline void UniformBuffer< T >::initialise()
	{
		CU_Require( m_renderSystem.hasCurrentRenderDevice() );
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_buffer = makeUniformBuffer< T >( device
			, m_count
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_flags
			, m_debugName );
	}

	template< typename T >
	inline void UniformBuffer< T >::cleanup()
	{
		CU_Require( m_renderSystem.hasCurrentRenderDevice() );
		m_buffer.reset();
	}

	template< typename T >
	inline bool UniformBuffer< T >::hasAvailable()const
	{
		return !m_available.empty();
	}

	template< typename T >
	inline uint32_t UniformBuffer< T >::allocate()
	{
		CU_Require( hasAvailable() );
		uint32_t result = *m_available.begin();
		m_available.erase( m_available.begin() );
		return result;
	}

	template< typename T >
	inline void UniformBuffer< T >::deallocate( uint32_t offset )
	{
		m_available.insert( offset );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		auto commandBuffer = commandPool.createCommandBuffer( true );
		upload( stagingBuffer
			, *commandBuffer
			, offset
			, flags
			, timer
			, index );
		auto fence = device->createFence();
		queue.submit( *commandBuffer
			, fence.get() );
		fence->wait( ashes::MaxTimeout );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto elemAlignedSize = getBuffer().getAlignedSize();
		auto data = getBuffer().getDatas().data();

		if ( auto dest = stagingBuffer.lock( 0u
			, m_count * elemAlignedSize
			, 0u ) )
		{
			auto buffer = dest;

			for ( auto & data : getBuffer().getDatas() )
			{
				std::memcpy( buffer, &data, sizeof( T ) );
				buffer += elemAlignedSize;
			}

			stagingBuffer.flush( 0u, m_count * elemAlignedSize );
			stagingBuffer.unlock();
		}

		copyBuffer( stagingBuffer
			, getBuffer().getUbo().getBuffer()
			, &getBuffer().getUbo().getBuffer()
			, commandBuffer
			, elemAlignedSize
			, m_count
			, offset
			, flags
			, timer
			, index );
	}

	template< typename T >
	inline void UniformBuffer< T >::download( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto elemAlignedSize = getBuffer().getAlignedSize();
		auto commandBuffer = commandPool.createCommandBuffer( true );
		copyBuffer( getBuffer().getUbo().getBuffer()
			, stagingBuffer
			, &getBuffer().getUbo().getBuffer()
			, commandBuffer
			, elemAlignedSize
			, m_count
			, offset
			, flags
			, timer
			, index );
		auto & device = getCurrentRenderDevice( m_renderSystem );
		auto fence = device->createFence();
		queue.submit( *commandBuffer
			, fence.get() );
		fence->wait( ashes::MaxTimeout );

		if ( auto dest = stagingBuffer.lock( 0u
			, m_count * elemAlignedSize
			, 0u ) )
		{
			auto buffer = dest;

			for ( auto & data : getBuffer().getDatas() )
			{
				std::memcpy( &data, buffer, sizeof( T ) );
				buffer += elemAlignedSize;
			}

			stagingBuffer.unlock();
		}
	}
}
