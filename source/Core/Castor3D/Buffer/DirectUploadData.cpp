#include "Castor3D/Buffer/DirectUploadData.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/CommandPool.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Queue.hpp>

#if C3D_DebugUpload
#	define traceUpload( x )\
	log::debug << x
#else
#	define traceUpload( x )
#endif

namespace c3d
{
	DirectUploadData::DirectUploadData( RenderDevice const & device
		, String debugName
		, ashes::CommandBuffer const & commandBuffer )
		: CommandBufferHolder{ nullptr }
		, UploadData{ device, c3d::move( debugName ), &commandBuffer }
	{
	}

	DirectUploadData::DirectUploadData( RenderDevice const & device
		, String debugName
		, ashes::CommandPool const & commandPool )
		: CommandBufferHolder{ commandPool.createCommandBuffer( toUtf8( debugName ) ) }
		, UploadData{ device, c3d::move( debugName ), CommandBufferHolder::getData().get() }
	{
	}

	DirectUploadData::~DirectUploadData()noexcept
	{
		for ( auto & buffer : m_buffers )
			buffer->destroy();
		m_buffers.clear();
	}

	void DirectUploadData::begin()
	{
		doBegin();
	}

	UploadData::SemaphoreUsed DirectUploadData::end( ashes::Queue const & queue
		, ashes::Fence const * fence
		, Milliseconds timeout )
	{
		doEnd();
		queue.submit( getCommandBuffer(), fence );

		if ( fence )
		{
			fence->wait( uint64_t( timeout.count() ) );
		}

		return {};
	}

	void DirectUploadData::cleanup()noexcept
	{
		doCleanup();
		CommandBufferHolder::setData( {} );
	}

	VkDeviceSize DirectUploadData::doUpload( BufferDataRange & data )
	{
		doUploadBuffer( data, nullptr, 0u );
		return data.srcSize;
	}

	VkDeviceSize DirectUploadData::doUpload( ImageDataRange & data )
	{
		traceUpload( "    Registering image upload commands: [" << makeString( data.dstImage->getName() )
			<< "], Layout: [" << data.dstLayout
			<< "], Range: [" << data.dstRange
			<< ", Upload Size: " << data.srcSize
			<< std::endl );
		auto mappedSize = ashes::getAlignedSize( data.srcSize
			, getDevice().renderSystem.getValue( GpuMin::eBufferMapSize ) );

		if ( auto const & buffer = *m_buffers.emplace_back( makeBufferBase( getDevice()
				, getDevice().renderSystem.getEngine()->getGraphResourceCache()
				, mappedSize
				, BufferUsageFlags::eTransferSrc
				, MemoryPropertyFlags::eHostVisible
				, getName() + cuT( "/StagingBuffer" ) ) );
			doCopyData( data.srcData, data.srcSize, *buffer.buffer, 0u ) )
		{
			doMemoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, buffer.buffer->makeTransferSource() );
			doUploadImage( data, *buffer.buffer, 0u );
		}

		return data.srcSize;
	}
}
