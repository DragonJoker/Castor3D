#include "Castor3D/Buffer/UniformBuffer.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
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
				, uint32_t( elemAlignedSize * count )
				, uint32_t( elemAlignedSize * offset ) );
			srcSrcStage = src.getCompatibleStageFlags();
			dstSrcStage = dst.getCompatibleStageFlags();

			if ( ubo == &src )
			{
				commandBuffer.memoryBarrier( srcSrcStage
					, flags
					, src.makeUniformBufferInput() );
			}
			else
			{
				commandBuffer.memoryBarrier( srcSrcStage
					, VK_PIPELINE_STAGE_HOST_BIT
					, src.makeHostWrite() );
			}

			if ( ubo == &dst )
			{
				commandBuffer.memoryBarrier( dstSrcStage
					, flags
					, dst.makeUniformBufferInput() );
			}
			else
			{
				commandBuffer.memoryBarrier( dstSrcStage
					, VK_PIPELINE_STAGE_HOST_BIT
					, dst.makeHostRead() );
			}

			timer.endPass( commandBuffer, index );
			commandBuffer.end();
		}
	}

	UniformBufferBase::UniformBufferBase( RenderSystem const & renderSystem
		, uint32_t elemCount
		, uint32_t elemSize
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String debugName
		, ashes::QueueShare sharingMode )
		: m_renderSystem{ renderSystem }
		, m_usage{ usage }
		, m_flags{ flags }
		, m_elemCount{ elemCount }
		, m_elemSize{ elemSize }
		, m_sharingMode{ std::move( sharingMode ) }
		, m_debugName{ std::move( debugName ) }
	{
		for ( uint32_t i = 0; i < m_elemCount; ++i )
		{
			m_available.insert( i );
		}

		if ( renderSystem.hasCurrentRenderDevice() )
		{
			initialise();
		}
	}

	uint32_t UniformBufferBase::initialise( ashes::QueueShare sharingMode )
	{
		m_sharingMode = sharingMode;
		return initialise();
	}

	uint32_t UniformBufferBase::initialise()
	{
		CU_Require( m_renderSystem.hasCurrentRenderDevice() );
		m_buffer.reset();
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_buffer = ashes::makeUniformBuffer( *device.device
			, m_elemCount
			, m_elemSize
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_sharingMode );
		setDebugObjectName( device, *m_buffer, m_debugName + "Ubo" );
		m_buffer->bindMemory( setupMemory( device, *m_buffer, m_flags, m_debugName + "Ubo" ) );
		m_transferFence = device.device->createFence();
		return uint32_t( m_buffer->getBuffer().getSize() );
	}

	void UniformBufferBase::cleanup()
	{
		CU_Require( m_renderSystem.hasCurrentRenderDevice() );
		m_buffer.reset();
	}

	bool UniformBufferBase::hasAvailable()const
	{
		return !m_available.empty();
	}

	uint32_t UniformBufferBase::allocate()
	{
		CU_Require( hasAvailable() );
		uint32_t result = *m_available.begin();
		m_available.erase( m_available.begin() );
		return result;
	}

	void UniformBufferBase::deallocate( uint32_t offset )
	{
		m_available.insert( offset );
	}

	void UniformBufferBase::upload( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, const void * data
		, size_t size
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		auto commandBuffer = commandPool.createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		upload( stagingBuffer
			, *commandBuffer
			, data
			, size
			, offset
			, flags
			, timer
			, index );
		queue.submit( *commandBuffer
			, m_transferFence.get() );
		m_transferFence->wait( ashes::MaxTimeout );
		m_transferFence->reset();
	}

	void UniformBufferBase::upload( ashes::BufferBase const & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, const void * data
		, size_t size
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto elemAlignedSize = getBuffer().getAlignedSize( m_elemSize );
		auto src = reinterpret_cast< const uint8_t * >( data );
		CU_Require( ( size % m_elemSize ) == 0 );
		auto count = size / m_elemSize;
		CU_Require( count <= m_elemCount - offset );

		if ( auto dest = stagingBuffer.lock( 0u
			, count * elemAlignedSize
			, 0u ) )
		{
			auto dst = dest;

			for ( uint32_t i = 0u; i < count; ++i )
			{
				std::memcpy( dst, src, m_elemSize );
				src += m_elemSize;
				dst += elemAlignedSize;
			}

			stagingBuffer.flush( 0u, count * elemAlignedSize );
			stagingBuffer.unlock();
		}

		copyBuffer( stagingBuffer
			, getBuffer().getBuffer()
			, &getBuffer().getBuffer()
			, commandBuffer
			, elemAlignedSize
			, count
			, offset
			, flags
			, timer
			, index );
	}

	void UniformBufferBase::download( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, void * data
		, size_t size
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		CU_Require( size >= m_elemCount * m_elemSize
			&& "Need a large enough buffer" );
		auto elemAlignedSize = getBuffer().getAlignedSize( m_elemSize );
		auto commandBuffer = commandPool.createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		copyBuffer( getBuffer().getBuffer()
			, stagingBuffer
			, &getBuffer().getBuffer()
			, *commandBuffer
			, elemAlignedSize
			, m_elemCount
			, offset
			, flags
			, timer
			, index );
		auto & device = getCurrentRenderDevice( m_renderSystem );
		queue.submit( *commandBuffer
			, m_transferFence.get() );
		m_transferFence->wait( ashes::MaxTimeout );
		m_transferFence->reset();
		auto dst = reinterpret_cast< uint8_t * >( data );

		if ( auto src = stagingBuffer.lock( 0u
			, m_elemCount * elemAlignedSize
			, 0u ) )
		{
			for ( uint32_t i = 0u; i < m_elemCount; ++i )
			{
				std::memcpy( dst, src, m_elemSize );
				src += elemAlignedSize;
				dst += m_elemSize;
			}

			stagingBuffer.unlock();
		}
	}
}
