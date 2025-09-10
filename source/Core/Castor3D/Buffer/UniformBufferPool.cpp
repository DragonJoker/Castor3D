#include "Castor3D/Buffer/UniformBufferPool.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

CU_ImplementSmartPtr( c3d, UniformBufferPool )

namespace c3d
{
	//*********************************************************************************************

	void copyBuffer( ashes::CommandBuffer const & commandBuffer
		, ashes::BufferBase const & src
		, ashes::BufferBase const & dst
		, DeviceSize offset
		, DeviceSize size
		, PipelineStageFlags flags )
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
			, getPipelineStageFlags( flags )
			, dst.makeUniformBufferInput() );
	}

	//*********************************************************************************************

	UniformBufferPool::UniformBufferPool( RenderDevice const & device
		, crg::ResourcesCache & resources
		, String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_resources{ resources }
		, m_debugName{ c3d::move( debugName ) }
	{
	}

	UniformBufferPool::~UniformBufferPool()noexcept
	{
		auto lock( makeUniqueLock( m_mutex ) );
		for ( auto & [_, buffers] : m_buffers )
		{
			for ( auto & buffer : buffers )
				buffer.buffer->cleanup();
		}

		m_buffers.clear();
	}

	AllocationStats UniformBufferPool::getAllocationStats()const noexcept
	{
		auto lock( makeUniqueLock( m_mutex ) );
		AllocationStats result{};

		for ( auto const & [_, buffers] : m_buffers )
		{
			for ( auto const & buffer : buffers )
			{
				if ( buffer.buffer->hasBuffer() )
				{
					result.total += buffer.buffer->getBuffer().getSize();
					result.available += buffer.buffer->getAvailable();
				}
			}
		}

		return result;
	}

	Vector< Pair< MemChunk, String > > UniformBufferPool::listAllocations()const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		Vector< Pair< MemChunk, String > > result;
		for ( auto const & [_, buffers] : m_buffers )
		{
			for ( auto const & buffer : buffers )
			{
				auto bufferAllocs = buffer.buffer->listAllocations();
				result.insert( result.end(), bufferAllocs.begin(), bufferAllocs.end() );
			}
		}

		return result;
	}

	void UniformBufferPool::upload( UploadData const & uploader )const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		auto & commandBuffer = uploader.getCommandBuffer();

		for ( auto & [id, buffers] : m_buffers )
		{
			for ( auto & buffer : buffers )
			{
				if ( buffer.buffer->hasAllocated() )
				{
					auto const & vkBuffer = *buffer.buffer->getBuffer().buffer;
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
		auto lock( makeUniqueLock( m_mutex ) );
		uint32_t result = 0u;

		for ( auto & [id, buffers] : m_buffers )
		{
			result += uint32_t( buffers.size() );
		}

		return result;
	}

	UniformBufferPool::BufferArray::iterator UniformBufferPool::doFindBuffer( UniformBufferPool::BufferArray & array
		, DeviceSize alignedSize )const
	{
		auto it = array.begin();

		while ( it != array.end() && !it->buffer->hasAvailable( alignedSize ) )
		{
			++it;
		}

		return it;
	}

	UniformBufferPool::BufferArray::iterator UniformBufferPool::doCreatePoolBuffer( MemoryPropertyFlags flags
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
			, m_resources
			, BufferUsageFlags::eTransferDst
			, flags
			, m_debugName
			, sharingMode );
		buffers.push_back( { m_currentUboIndex, c3d::move( buffer ) } );
		++m_currentUboIndex;
		auto itB = std::next( buffers.begin()
			, ptrdiff_t( buffers.size() - 1 ) );
		m_maxUboSize = itB->buffer->initialise( m_device );
		return itB;
	}

	//*********************************************************************************************
}
