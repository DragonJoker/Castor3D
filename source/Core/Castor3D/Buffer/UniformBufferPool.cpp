#include "Castor3D/Buffer/UniformBufferPool.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

CU_ImplementSmartPtr( castor3d, UniformBufferPool )

namespace castor3d
{
	//*********************************************************************************************

	void copyBuffer( ashes::CommandBuffer const & commandBuffer
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
			, size
			, offset
			, 0u );
		dstSrcStage = dst.getCompatibleStageFlags();
		commandBuffer.memoryBarrier( dstSrcStage
			, flags
			, dst.makeUniformBufferInput() );
	}

	//*********************************************************************************************

	UniformBufferPool::UniformBufferPool( RenderDevice const & device
		, castor::String debugName )
		: castor::OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_debugName{ std::move( debugName ) }
	{
	}

	void UniformBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	void UniformBufferPool::upload( UploadData const & uploader )const
	{
		auto & commandBuffer = uploader.getCommandBuffer();

		for ( auto & [id, buffers] : m_buffers )
		{
			for ( auto & buffer : buffers )
			{
				if ( buffer.buffer->hasAllocated() )
				{
					auto const & vkBuffer = buffer.buffer->getBuffer().getBuffer();
					auto curFlags = vkBuffer.getCompatibleStageFlags();
					auto barrier = vkBuffer.makeHostWrite();

					if ( curFlags != VK_PIPELINE_STAGE_HOST_BIT )
					{
						commandBuffer.memoryBarrier( curFlags
							, VK_PIPELINE_STAGE_HOST_BIT
							, barrier );
					}

					buffer.buffer->flush();

					if ( curFlags != VK_PIPELINE_STAGE_HOST_BIT )
					{
						commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
							, curFlags
							, vkBuffer.makeMemoryTransitionBarrier( barrier.srcAccessMask ) );
					}
					else
					{
						commandBuffer.memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
							, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
							, vkBuffer.makeUniformBufferInput() );
					}
				}
			}
		}
	}

	uint32_t UniformBufferPool::getBufferCount()const
	{
		uint32_t result = 0u;

		for ( auto & [id, buffers] : m_buffers )
		{
			result += uint32_t( buffers.size() );
		}

		return result;
	}

	UniformBufferPool::BufferArray::iterator UniformBufferPool::doFindBuffer( UniformBufferPool::BufferArray & array
		, VkDeviceSize alignedSize )const
	{
		auto it = array.begin();

		while ( it != array.end() && !it->buffer->hasAvailable( alignedSize ) )
		{
			++it;
		}

		return it;
	}

	UniformBufferPool::BufferArray::iterator UniformBufferPool::doCreatePoolBuffer( VkMemoryPropertyFlags flags
		, UniformBufferPool::BufferArray & buffers )
	{
		auto const & renderSystem = *getRenderSystem();
		auto const & device = renderSystem.getRenderDevice();
		ashes::QueueShare sharingMode
		{
			{
				device.getGraphicsQueueFamilyIndex(),
				device.getComputeQueueFamilyIndex(),
				device.getTransferQueueFamilyIndex(),
			}
		};
		auto buffer = makePoolUniformBuffer( renderSystem
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, flags
			, m_debugName
			, sharingMode );
		buffers.push_back( { m_currentUboIndex, std::move( buffer ) } );
		++m_currentUboIndex;
		auto itB = std::next( buffers.begin()
			, ptrdiff_t( buffers.size() - 1 ) );
		m_maxUboSize = itB->buffer->initialise( m_device );
		return itB;
	}

	//*********************************************************************************************
}
