#include "Castor3D/Buffer/StagedUploadData.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/FramePassTimer.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Sync/Queue.hpp>

namespace castor3d
{
	namespace upload
	{
		static u32 constexpr MaxBufferLifeTime = 20u;
	}

	StagedUploadData::StagedUploadData( RenderDevice const & device
		, castor::String debugName
		, ashes::CommandBufferPtr commandBuffer )
		: CommandBufferHolder{ castor::move( commandBuffer ) }
		, UploadData{ device, castor::move( debugName ), CommandBufferHolder::getData().get() }
		, m_buffers{ FrameBuffers{ device->createSemaphore( castor::toUtf8( m_debugName ) ) }
			, FrameBuffers{ device->createSemaphore( castor::toUtf8( m_debugName ) ) } }
		, m_cpuBuffers{ &m_buffers[0] }
		, m_gpuBuffers{ &m_buffers[0] }
		, m_timer{ castor::makeUnique< crg::FramePassTimer >( device.makeContext(), "Upload", crg::TimerScope::eUpdate ) }
	{
		m_device.renderSystem.getEngine()->registerTimer( cuT( "Upload" ), *m_timer );
	}

	StagedUploadData::~StagedUploadData()noexcept
	{
		m_device.renderSystem.getEngine()->unregisterTimer( cuT( "Upload" ), *m_timer );
		VkDeviceSize totalSize{};

		for ( auto [buffer, mapped] : m_wholeBuffers )
		{
			buffer->unlock();
			totalSize += buffer->getSize();
		}

		m_buffers = { FrameBuffers{}, FrameBuffers{} };
		log::info << "  Staging Buffers total allocated size: " << totalSize << " bytes" << std::endl;
	}

	void StagedUploadData::doBegin()
	{
		m_cpuBlock = castor::make_unique< crg::FramePassTimerBlock >( m_timer->start() );
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
	}

	void StagedUploadData::doPreprocess( castor::Vector< BufferDataRange > *& pendingBuffers
		, castor::Vector< ImageDataRange > *& pendingImages )
	{
		auto const & engine = *m_device.renderSystem.getEngine();
		m_commandBuffer->beginDebugBlock( { "Buffers Upload"
			, makeFloatArray( engine.getNextRainbowColour() ) } );
		m_timer->beginPass( *m_commandBuffer );

		for ( auto const & [range, offset] : m_cpuBuffers->bufferOffsets )
		{
			doPutBuffer( m_cpuBuffers->pool, offset );
		}

		for ( auto const & [range, offset] : m_cpuBuffers->imageOffsets )
		{
			doPutBuffer( m_cpuBuffers->pool, offset );
		}

		m_cpuBuffers->buffers.clear();
		m_cpuBuffers->bufferOffsets.clear();
		m_cpuBuffers->imageOffsets.clear();
		m_cpuBuffers->pendingBuffers = m_pendingBuffers;
		m_cpuBuffers->pendingImages = m_pendingImages;
		m_cpuBuffers->currentSize = 0u;

		for ( auto const & pending : m_cpuBuffers->pendingBuffers )
		{
			auto const & offset = m_cpuBuffers->bufferOffsets.try_emplace( &pending
				, doGetBuffer( m_cpuBuffers->pool, pending.srcSize ) ).first->second;

			if ( offset.getAllocSize() != ashes::getAlignedSize( offset.getAllocSize(), m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Chunk size should be aligned"
					<< ": size = " << offset.getAllocSize()
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Chunk size should be aligned" );
			}

			if ( offset.getOffset() != ashes::getAlignedSize( offset.getOffset(), m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Chunk offset should be aligned"
					<< ": offset = " << offset.getOffset()
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Chunk offset should be aligned" );
			}

			auto & res = m_cpuBuffers->buffers.try_emplace( offset.buffer ).first->second;
			res.offset = std::min( res.offset, offset.getOffset() );

			if ( res.offset != ashes::getAlignedSize( res.offset, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Offset should be aligned"
					<< ": offset = " << res.offset
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Offset should be aligned" );
			}

			res.range = std::max( res.range
				, std::min( offset.getOffset() + offset.getAllocSize()
					, offset.buffer->getSize() ) );

			if ( res.range != ashes::getAlignedSize( res.range, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Range should be aligned"
					<< ": range = " << res.range
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Range should be aligned" );
			}

			m_cpuBuffers->currentSize += pending.srcSize;
			auto [it, inserted] = m_wholeBuffers.try_emplace( offset.buffer );

			if ( inserted )
			{
				it->second = offset.buffer->lock( 0u, ashes::WholeSize, 0u );
			}

			res.mapped = it->second;
		}

		for ( auto & pending : m_cpuBuffers->pendingImages )
		{
			auto const & offset = m_cpuBuffers->imageOffsets.try_emplace( &pending
				, doGetBuffer( m_cpuBuffers->pool, pending.srcSize ) ).first->second;

			if ( offset.getAllocSize() != ashes::getAlignedSize( offset.getAllocSize(), m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Chunk size should be aligned"
					<< ": size = " << offset.getAllocSize()
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Chunk size should be aligned" );
			}

			if ( offset.getOffset() != ashes::getAlignedSize( offset.getOffset(), m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Chunk offset should be aligned"
					<< ": offset = " << offset.getOffset()
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Chunk offset should be aligned" );
			}

			auto & res = m_cpuBuffers->buffers.try_emplace( offset.buffer ).first->second;
			res.offset = std::min( res.offset, offset.getOffset() );

			if ( res.offset != ashes::getAlignedSize( res.offset, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Offset should be aligned"
					<< ": offset = " << res.offset
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Offset should be aligned" );
			}

			res.range = std::max( res.range
				, std::min( offset.getOffset() + offset.getAllocSize()
					, offset.buffer->getSize() ) );

			if ( res.range != ashes::getAlignedSize( res.range, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) )
			{
				log::error << "StagedUploadBuffer: Range should be aligned"
					<< ": range = " << res.range
					<< ", align = " << m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) << std::endl;
				CU_Failure( "Range should be aligned" );
			}

			m_cpuBuffers->currentSize += pending.srcSize;
			auto [it, inserted] = m_wholeBuffers.try_emplace( offset.buffer );

			if ( inserted )
			{
				it->second = offset.buffer->lock( 0u, ashes::WholeSize, 0u );
			}

			res.mapped = it->second;
		}

		m_cpuBuffers->buffersCount = m_cpuBuffers->buffers.size();
		pendingBuffers = &m_cpuBuffers->pendingBuffers;
		pendingImages = &m_cpuBuffers->pendingImages;
	}

	VkDeviceSize StagedUploadData::doUpload( BufferDataRange & data )
	{
		auto offsetIt = m_cpuBuffers->bufferOffsets.find( &data );

		if ( offsetIt == m_cpuBuffers->bufferOffsets.end() )
		{
			log::error << "Buffer range was not prepared.\n";
			CU_Failure( "Buffer range was not prepared." );
			return 0u;
		}

		auto const & offset = offsetIt->second;
		auto bufferIt = m_cpuBuffers->buffers.find( offset.buffer );

		if ( offset.getOffset() + data.srcSize > offset.buffer->getSize() )
		{
			log::error << "StagedUploadBuffer: Trying to copy more than there can be in staging [" << castor::makeString( offset.buffer->getName() )
				<< "] buffer: offset = " << offset.getOffset()
				<< ", size = " << data.srcSize << std::endl;
			CU_Failure( "Trying to copy more than there can be in staging buffer" );
		}

		if ( data.dstOffset >= data.dstBuffer->getSize() )
		{
			log::error << "StagedUploadBuffer: Trying to copy at invalid offset for target [" << castor::makeString( data.dstBuffer->getName() )
				<< "] buffer: dstOffset = " << data.dstOffset << std::endl;
			CU_Failure( "Trying to copy at invalid offset for target buffer" );
		}

		if ( data.dstOffset + data.srcSize > data.dstBuffer->getSize() )
		{
			log::error << "StagedUploadBuffer: Trying to copy more than there is in target [" << castor::makeString( data.dstBuffer->getName() )
				<< "] buffer: dstOffset = " << data.dstOffset
				<< ", size = " << data.srcSize << std::endl;
			CU_Failure( "Trying to copy more than there is in target buffer" );
		}

		std::memcpy( bufferIt->second.mapped + offset.getOffset(), data.srcData, data.srcSize );
		return data.srcSize;
	}

	VkDeviceSize StagedUploadData::doUpload( ImageDataRange & data )
	{
		auto offsetIt = m_cpuBuffers->imageOffsets.find( &data );

		if ( offsetIt == m_cpuBuffers->imageOffsets.end() )
		{
			log::error << "Image range was not prepared.\n";
			CU_Failure( "Image range was not prepared." );
			return 0u;
		}

		auto const & offset = offsetIt->second;
		auto bufferIt = m_cpuBuffers->buffers.find( offset.buffer );

		if ( offset.getOffset() + data.srcSize > offset.buffer->getSize() )
		{
			log::error << "StagedUploadImage: Trying to copy more than there can be in staging [" << castor::makeString( offset.buffer->getName() )
				<< "] buffer: offset = " << offset.getOffset()
				<< ", size = " << data.srcSize << std::endl;
			CU_Failure( "Trying to copy more than there can be in staging buffer" );
		}

		if ( auto imgSize = data.dstImage->getMemoryRequirements().size;
			data.srcSize > imgSize )
		{
			log::error << "StagedUploadImage: Trying to copy more than there can be in image [" << castor::makeString( data.dstImage->getName() )
				<< "] device memory: size = " << data.srcSize << std::endl;
			CU_Failure( "Trying to copy more than there can be in image" );
		}

		if ( data.dstRange.baseArrayLayer >= data.dstImage->getLayerCount() )
		{
			log::error << "StagedUploadImage: Trying to copy to invalid base array layer for image [" << castor::makeString( data.dstImage->getName() )
				<< "]: baseArrayLayer = " << data.dstRange.baseArrayLayer << std::endl;
			CU_Failure( "Trying to copy to invalid base array layer for image" );
		}

		if ( data.dstRange.baseArrayLayer + data.dstRange.layerCount > data.dstImage->getLayerCount() )
		{
			log::error << "StagedUploadImage: Trying to copy to invalid array layers for image [" << castor::makeString( data.dstImage->getName() )
				<< "]: baseArrayLayer = " << data.dstRange.baseArrayLayer
				<< ", layerCount = " << data.dstRange.layerCount << std::endl;
			CU_Failure( "Trying to copy to invalid array layers for image" );
		}

		if ( data.dstRange.baseMipLevel >= data.dstImage->getMipmapLevels() )
		{
			log::error << "StagedUploadImage: Trying to copy to invalid base mip level for image [" << castor::makeString( data.dstImage->getName() )
				<< "]: baseMipLevel = " << data.dstRange.baseArrayLayer << std::endl;
			CU_Failure( "Trying to copy to invalid base mip level for image" );
		}

		if ( data.dstRange.baseMipLevel + data.dstRange.levelCount > data.dstImage->getMipmapLevels() )
		{
			log::error << "StagedUploadImage: Trying to copy to invalid mip levels for image [" << castor::makeString( data.dstImage->getName() )
				<< "]: baseMipLevel = " << data.dstRange.baseMipLevel
				<< ", levelCount = " << data.dstRange.levelCount << std::endl;
			CU_Failure( "Trying to copy to invalid mip levels for image" );
		}

		std::memcpy( bufferIt->second.mapped + offset.getOffset(), data.srcData, data.srcSize );
		return data.srcSize;
	}

	void StagedUploadData::doPostprocess()
	{
		if ( !m_gpuBuffers->pendingBuffers.empty()
			|| !m_gpuBuffers->pendingImages.empty() )
		{
			for ( auto const & [buffer, bounds] : m_gpuBuffers->buffers )
			{
				buffer->flush( bounds.offset, bounds.range - bounds.offset );
				m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
					, VK_PIPELINE_STAGE_TRANSFER_BIT
					, buffer->makeTransferSource() );
			}

			for ( auto const & [upload, offset] : m_gpuBuffers->bufferOffsets )
			{
				auto const & srcBuffer = *offset.buffer;
				doUploadBuffer( *upload
					, &srcBuffer
					, offset.getOffset() );
			}

			for ( auto const & [upload, offset] : m_gpuBuffers->imageOffsets )
			{
				doUploadImage( *upload
					, *offset.buffer
					, offset.getOffset() );
			}

			for ( auto const & [buffer, bounds] : m_gpuBuffers->buffers )
			{
				m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, VK_PIPELINE_STAGE_HOST_BIT
					, buffer->makeHostWrite() );
			}
		}

		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->endDebugBlock();
		m_timer->notifyPassRender( m_frameIndex );
	}

	UploadData::SemaphoreUsed StagedUploadData::doEnd( ashes::Queue const & queue
		, ashes::Fence const * fence
		, castor::Milliseconds timeout )
	{
		m_commandBuffer->end();
		m_cpuBlock = {};
		VkFence vkFence = fence ? *fence : VkFence{};
		queue.submit( getCommandBuffer()
			, ( m_gpuBuffers->used
				? VkSemaphore{ VK_NULL_HANDLE }
				: *m_gpuBuffers->semaphore )
			, ( m_gpuBuffers->used
				? VkPipelineStageFlagBits{}
				: VK_PIPELINE_STAGE_TRANSFER_BIT )
			, *m_gpuBuffers->semaphore
			, vkFence );
		UploadData::SemaphoreUsed result{ m_gpuBuffers->semaphore.get()
			, &m_gpuBuffers->used
			, m_gpuBuffers->currentSize
			, m_gpuBuffers->buffersCount };

		castor::swap( m_cpuBuffers, m_gpuBuffers );
		m_frameIndex = 1u - m_frameIndex;

		if ( fence )
		{
			fence->wait( uint64_t( timeout.count() ) );
			fence->reset();
		}

		auto it = m_gpuBuffers->pool.begin();

		while ( it != m_gpuBuffers->pool.end() )
		{
			--it->lifetime;

			if ( it->lifetime == 0u )
			{
				if ( auto wit = m_wholeBuffers.find( &it->buffer->getBuffer() );
					wit != m_wholeBuffers.end() )
				{
					wit->first->unlock();
					m_wholeBuffers.erase( wit );
				}
				else
				{
					log::error << "StagedUpload: Unexpected unmapped buffer" << std::endl;
					CU_Failure( "StagedUpload: Unexpected unmapped buffer" );
				}

				log::debug << cuT( "Releasing staging buffer [" ) << castor::makeString( it->buffer->getBuffer().getName() ) << cuT( "]" ) << std::endl;
				it = m_gpuBuffers->pool.erase( it );
			}
			else
			{
				++it;
			}
		}

		return result;
	}

	StagedUploadData::GpuBufferOffset StagedUploadData::doGetBuffer( BufferArray & pool
		, VkDeviceSize size )
	{
		GpuBufferOffset result;
		auto it = pool.begin();

		while ( it != pool.end()
			&& !it->buffer->hasAvailable( size ) )
		{
			++it;
		}

		if ( it == pool.end() )
		{
			VkDeviceSize maxCount = BaseObjectPoolBufferCount * 64u;

			while ( maxCount < size )
			{
				maxCount *= 2u;
			}

			StagingBuffer buffer{ castor::makeUnique< GpuPackedBaseBuffer >( m_device
				, VkBufferUsageFlags{ VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT }
				, VkMemoryPropertyFlags{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
				, m_debugName + cuT( "Staging" ) + castor::string::toString( pool.size() )
				, ashes::QueueShare{}
				, GpuBufferPackedAllocator{ uint32_t( maxCount )
				, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) } ) };
			pool.emplace_back( castor::move( buffer ) );
			it = std::next( pool.begin()
				, ptrdiff_t( pool.size() - 1u ) );
		}

		result.buffer = &it->buffer->getBuffer();
		result.chunk = it->buffer->allocate( size );
		it->lifetime = upload::MaxBufferLifeTime;

		if ( result.getOffset() + size > result.buffer->getSize() )
		{
			log::error << "StagedUploadBuffer: Retrieved invalid offset from [" << castor::makeString( result.buffer->getName() )
				<< "] buffer: offset = " << result.getOffset()
				<< ", size = " << size << std::endl;
			CU_Failure( "Retrieved invalid offset from buffer" );
		}

		return result;
	}

	void StagedUploadData::doPutBuffer( BufferArray & pool
		, GpuBufferOffset const & bufferOffset )const noexcept
	{
		auto it = std::find_if( pool.begin()
			, pool.end()
			, [&bufferOffset]( StagingBuffer const & lookup )
			{
				return &lookup.buffer->getBuffer() == bufferOffset.buffer;
			} );
		CU_Require( it != pool.end() );
		it->buffer->deallocate( bufferOffset.chunk );
	}
}
