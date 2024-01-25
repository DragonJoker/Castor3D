#include "Castor3D/Buffer/UniformBufferBase.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <algorithm>

CU_ImplementSmartPtr( castor3d, UniformBufferBase )

namespace castor3d
{
	namespace bufferunf
	{
		inline void doCopyBuffer( ashes::BufferBase const & src
			, ashes::BufferBase const & dst
			, ashes::BufferBase const * ubo
			, ashes::CommandBuffer const & commandBuffer
			, VkDeviceSize elemAlignedSize
			, VkDeviceSize count
			, VkDeviceSize offset
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
				, elemAlignedSize * count
				, elemAlignedSize * offset
				, 0u );
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
		}

		inline void copyBuffer( ashes::BufferBase const & src
			, ashes::BufferBase const & dst
			, ashes::BufferBase const * ubo
			, ashes::CommandBuffer const & commandBuffer
			, VkDeviceSize elemAlignedSize
			, VkDeviceSize count
			, VkDeviceSize offset
			, VkPipelineStageFlags flags
			, FramePassTimer & timer )
		{
			commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			timer.beginPass( commandBuffer );
			doCopyBuffer( src
				, dst
				, ubo
				, commandBuffer
				, elemAlignedSize
				, count
				, offset
				, flags );
			timer.endPass( commandBuffer );
			commandBuffer.end();
		}

		inline void copyBuffer( ashes::BufferBase const & src
			, ashes::BufferBase const & dst
			, ashes::BufferBase const * ubo
			, ashes::CommandBuffer const & commandBuffer
			, VkDeviceSize elemAlignedSize
			, VkDeviceSize count
			, VkDeviceSize offset
			, VkPipelineStageFlags flags )
		{
			commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			doCopyBuffer( src
				, dst
				, ubo
				, commandBuffer
				, elemAlignedSize
				, count
				, offset
				, flags );
			commandBuffer.end();
		}
	}

	UniformBufferBase::UniformBufferBase( RenderSystem const & renderSystem
		, VkDeviceSize elemCount
		, VkDeviceSize elemSize
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String debugName
		, ashes::QueueShare sharingMode )
		: m_usage{ usage }
		, m_flags{ flags }
		, m_elemCount{ uint32_t( elemCount ) }
		, m_elemSize{ uint32_t( elemSize ) }
		, m_sharingMode{ castor::move( sharingMode ) }
		, m_debugName{ castor::move( debugName ) }
	{
		for ( uint32_t i = 0; i < m_elemCount; ++i )
		{
			m_available.insert( i );
		}

		initialise( renderSystem.getRenderDevice() );
	}

	uint32_t UniformBufferBase::initialise( RenderDevice const & device
		, ashes::QueueShare sharingMode )
	{
		m_sharingMode = castor::move( sharingMode );
		return initialise( device );
	}

	uint32_t UniformBufferBase::initialise( RenderDevice const & device )
	{
		m_buffer.reset();
		m_buffer = ashes::makeUniformBuffer( *device.device
			, castor::toUtf8( m_debugName + cuT( "Ubo" ) )
			, m_elemCount
			, m_elemSize
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_sharingMode );
		m_buffer->bindMemory( setupMemory( device, *m_buffer, m_flags, m_debugName + cuT( "Ubo" ) ) );
		m_transferFence = device.device->createFence( castor::toUtf8( m_debugName + cuT( "Transfer" ) ) );
		return uint32_t( m_buffer->getBuffer().getSize() );
	}

	void UniformBufferBase::cleanup()noexcept
	{
		m_buffer.reset();
	}

	bool UniformBufferBase::hasAvailable()const noexcept
	{
		return !m_available.empty();
	}

	uint32_t UniformBufferBase::allocate()noexcept
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
		, VkPipelineStageFlags flags )const
	{
		auto commandBuffer = commandPool.createCommandBuffer( "UniformBufferUpload"
			, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		upload( stagingBuffer
			, *commandBuffer
			, data
			, size
			, offset
			, flags );
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
		, VkPipelineStageFlags flags )const
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

		bufferunf::copyBuffer( stagingBuffer
			, getBuffer().getBuffer()
			, &getBuffer().getBuffer()
			, commandBuffer
			, elemAlignedSize
			, count
			, offset
			, flags );
	}

	void UniformBufferBase::upload( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, const void * data
		, size_t size
		, uint32_t offset
		, VkPipelineStageFlags flags
		, FramePassTimer & timer )const
	{
		auto commandBuffer = commandPool.createCommandBuffer( "UniformBufferUpload"
			, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		upload( stagingBuffer
			, *commandBuffer
			, data
			, size
			, offset
			, flags
			, timer );
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
		, FramePassTimer & timer )const
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

		bufferunf::copyBuffer( stagingBuffer
			, getBuffer().getBuffer()
			, &getBuffer().getBuffer()
			, commandBuffer
			, elemAlignedSize
			, count
			, offset
			, flags
			, timer );
	}

	void UniformBufferBase::download( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, void * data
		, size_t size
		, uint32_t offset
		, VkPipelineStageFlags flags
		, FramePassTimer & timer )const
	{
		CU_Require( size >= size_t( m_elemCount ) * m_elemSize
			&& "Need a large enough buffer" );
		auto elemAlignedSize = getBuffer().getAlignedSize( m_elemSize );
		auto commandBuffer = commandPool.createCommandBuffer( "UniformBufferDownload"
			, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		bufferunf::copyBuffer( getBuffer().getBuffer()
			, stagingBuffer
			, &getBuffer().getBuffer()
			, *commandBuffer
			, elemAlignedSize
			, m_elemCount
			, offset
			, flags
			, timer );
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
