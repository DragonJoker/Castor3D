#include "Castor3D/Buffer/UniformBufferPool.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

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
			auto dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( dstSrcStage
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, dst.makeTransferDestination() );
			commandBuffer.copyBuffer( src
				, dst
				, uint32_t( size )
				, uint32_t( offset ) );
			dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( dstSrcStage
				, flags
				, dst.makeUniformBufferInput() );
		}
	}

	UniformBufferPool::UniformBufferPool( RenderSystem & renderSystem
		, RenderDevice const & device
		, castor::String debugName )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, m_device{ device }
		, m_debugName{ std::move( debugName ) }
	{
	}

	UniformBufferPool::~UniformBufferPool()
	{
	}

	void UniformBufferPool::cleanup()
	{
		m_buffers.clear();

		if ( m_stagingBuffer )
		{
			m_stagingBuffer->getBuffer().unlock();
			m_stagingBuffer.reset();
		}
	}

	void UniformBufferPool::upload( ashes::CommandBuffer const & commandBuffer )const
	{
		if ( m_stagingBuffer )
		{
			m_stagingBuffer->getBuffer().flush( 0u, m_currentUboIndex * m_maxUboSize );
			auto stgSrcStage = m_stagingBuffer->getBuffer().getCompatibleStageFlags();
			commandBuffer.memoryBarrier( stgSrcStage
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_stagingBuffer->getBuffer().makeTransferSource() );

			for ( auto & bufferIt : m_buffers )
			{
				for ( auto & buffer : bufferIt.second )
				{
					if ( buffer.buffer->hasAllocated() )
					{
						details::copyBuffer( commandBuffer
							, m_stagingBuffer->getBuffer()
							, buffer.buffer->getBuffer().getBuffer()
							, buffer.index * m_maxUboSize
							, m_maxUboSize
							, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
					}
				}
			}

			auto stgDstStage = m_stagingBuffer->getBuffer().getCompatibleStageFlags();
			commandBuffer.memoryBarrier( stgDstStage
				, VK_PIPELINE_STAGE_HOST_BIT
				, m_stagingBuffer->getBuffer().makeHostWrite() );
		}
	}

	uint32_t UniformBufferPool::getBufferCount()const
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

	UniformBufferPool::BufferArray::iterator UniformBufferPool::doFindBuffer( UniformBufferPool::BufferArray & array
		, VkDeviceSize alignedSize )
	{
		auto it = array.begin();

		while ( it != array.end() && !it->buffer->hasAvailable( alignedSize ) )
		{
			++it;
		}

		return it;
	}

	void UniformBufferPool::doCreateStagingBuffer()
	{
		auto & renderSystem = *getRenderSystem();
		auto & device = *renderSystem.getMainRenderDevice();
		ashes::QueueShare sharingMode
		{
			{
				device.getGraphicsQueueFamilyIndex(),
				device.getComputeQueueFamilyIndex(),
				device.getTransferQueueFamilyIndex(),
			}
		};
		auto & properties = renderSystem.getProperties();
		auto maxSize = std::min( 65536u, properties.limits.maxUniformBufferRange );
		auto elementSize = properties.limits.minUniformBufferOffsetAlignment;
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

	UniformBufferPool::BufferArray::iterator UniformBufferPool::doCreatePoolBuffer( VkMemoryPropertyFlags flags
		, UniformBufferPool::BufferArray & buffers )
	{
		auto & renderSystem = *getRenderSystem();
		auto & device = *renderSystem.getMainRenderDevice();
		ashes::QueueShare sharingMode
		{
			{
				device.getGraphicsQueueFamilyIndex(),
				device.getComputeQueueFamilyIndex(),
				device.getTransferQueueFamilyIndex(),
			}
		};
		auto index = m_maxUboSize * m_currentUboIndex;
		auto buffer = makePoolUniformBuffer( renderSystem
			, castor::makeArrayView( m_stagingData + index
				, m_stagingData + index + m_maxUboSize )
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, flags
			, m_debugName
			, sharingMode );
		buffers.push_back( { m_currentUboIndex, std::move( buffer ) } );
		++m_currentUboIndex;
		auto itB = std::next( buffers.begin(), buffers.size() - 1 );
		m_maxUboSize = itB->buffer->initialise( m_device );
		return itB;
	}
}
