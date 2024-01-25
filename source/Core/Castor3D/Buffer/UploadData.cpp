#include "Castor3D/Buffer/UploadData.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/Image.hpp>

CU_ImplementSmartPtr( castor3d, UploadData )

#if C3D_DebugUpload
#	define traceUpload( x )\
	log::debug << x
#else
#	define traceUpload( x )
#endif

namespace castor3d
{
	castor::OutputStream & operator<<( castor::OutputStream & stream, VkImageSubresourceRange const & rhs )
	{
		stream << rhs.aspectMask
			<< ", Array[" << rhs.baseArrayLayer << "/" << rhs.layerCount << "]"
			<< ", Mips[" << rhs.baseMipLevel << "/" << rhs.levelCount << "]";
		return stream;
	}

	UploadData::UploadData( RenderDevice const & device
		, castor::String debugName
		, ashes::CommandBuffer const * commandBuffer )
		: m_device{ device }
		, m_debugName{ castor::move( debugName ) }
		, m_commandBuffer{ commandBuffer }
	{
	}

	void UploadData::begin()
	{
		doBegin();
	}

	void UploadData::pushUpload( void const * srcData
		, VkDeviceSize srcSize
		, ashes::BufferBase const & dstBuffer
		, VkDeviceSize dstOffset
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags )
	{
		if ( !srcSize || !srcData )
		{
			return;
		}

		BufferDataRange upload{ srcData, srcSize, &dstBuffer, dstOffset, dstAccessFlags, dstPipelineFlags };
		auto it = std::lower_bound( m_pendingBuffers.begin()
			, m_pendingBuffers.end()
			, upload
			, []( BufferDataRange const & lhs, BufferDataRange const & rhs )noexcept
			{
				return lhs.dstBuffer < rhs.dstBuffer
					|| ( lhs.dstBuffer == rhs.dstBuffer && lhs.dstOffset < rhs.dstOffset );
			} );
		m_pendingBuffers.emplace( it, castor::move( upload ) );
	}

	void UploadData::pushUpload( void const * srcData
		, VkDeviceSize srcSize
		, ashes::Image const & dstImage
		, castor::ImageLayout dstLayout
		, VkImageSubresourceRange dstRange
		, VkImageLayout dstImageLayout
		, VkPipelineStageFlags dstPipelineFlags )
	{
		if ( !srcSize || !srcData )
		{
			return;
		}

		ImageDataRange upload{ srcData, srcSize, &dstImage, castor::move( dstLayout ), dstRange, dstImageLayout, dstPipelineFlags };
		auto it = std::lower_bound( m_pendingImages.begin()
			, m_pendingImages.end()
			, upload
			, []( ImageDataRange const & lhs, ImageDataRange const & rhs )noexcept
			{
				return lhs.dstImage < rhs.dstImage
					|| ( lhs.dstImage == rhs.dstImage
						&& ( lhs.dstRange.baseArrayLayer < rhs.dstRange.baseArrayLayer
							|| ( lhs.dstRange.baseArrayLayer == rhs.dstRange.baseArrayLayer
								&& lhs.dstRange.baseMipLevel < rhs.dstRange.baseMipLevel ) ) );
			} );
		m_pendingImages.emplace( it, castor::move( upload ) );
	}

	void UploadData::process()
	{
		castor::Vector< BufferDataRange > * pendingBuffers;
		castor::Vector< ImageDataRange > * pendingImages;
		traceUpload( "Start upload" << std::endl );
		doPreprocess( pendingBuffers, pendingImages );
#if C3D_DebugUpload
		VkDeviceSize size{};

		for ( auto & upload : *pendingBuffers )
		{
			size += doUpload( upload );
		}

		for ( auto & upload : *pendingImages )
		{
			size += doUpload( upload );
		}

		doPostprocess();
		traceUpload( "End upload, total size: " << size << std::endl );
#else
		for ( auto & upload : *pendingBuffers )
		{
			doUpload( upload );
		}

		for ( auto & upload : *pendingImages )
		{
			doUpload( upload );
		}

		doPostprocess();
#endif
		m_pendingBuffers.clear();
		m_pendingImages.clear();
	}

	UploadData::SemaphoreUsed UploadData::end( ashes::Queue const & queue
		, ashes::Fence const * fence
		, castor::Milliseconds timeout )
	{
		return doEnd( queue, fence, timeout );
	}

	bool UploadData::doCopyData( void const * data
		, VkDeviceSize size
		, ashes::BufferBase const & dstBuffer
		, VkDeviceSize dstOffset )const
	{
		uint32_t const align = m_device.renderSystem.getValue( GpuMin::eBufferMapSize );

		if ( size != ashes::WholeSize
			&& dstOffset + size > dstBuffer.getSize() )
		{
			log::error << cuT( "StagedUpload: Trying to copy more than there can be in dst [" ) << castor::makeString( dstBuffer.getName() )
				<< cuT( "] buffer: dstOffset = " ) << dstOffset
				<< cuT( ", size = " ) << size << std::endl;
			CU_Failure( "Trying to copy more than there can be in dst buffer" );
		}

		bool isFullSizeMap = ( size == ashes::WholeSize
			|| ( dstOffset == 0u && size >= dstBuffer.getSize() ) );
		auto alignedOffset = isFullSizeMap
			? ( dstOffset = 0u ) // Intended assignment here
			: ashes::getAlignedSize( dstOffset, align );
		auto alignedSize = isFullSizeMap
			? ashes::WholeSize
			: ashes::getAlignedSize( size, align );
		auto offsetAlignment = ( ( dstOffset == alignedOffset )
			? 0u
			: align );
		auto mappedOffset = alignedOffset - offsetAlignment;

		if ( dstOffset < mappedOffset )
		{
			log::error << cuT( "StagedUpload: Mapped offset " ) << mappedOffset
				<< cuT( " and destination offset " ) << dstOffset
				<< cuT( " are invalid." ) << std::endl;
			CU_Failure( "Invalid offsets for upload" );
		}

		auto offsetDiff = uint32_t( std::max( int64_t( 0 ), int64_t( dstOffset ) - int64_t( mappedOffset ) ) );
		auto mappedSize = ( ( alignedSize == alignedOffset ) ? 0u : offsetAlignment ) // If offset and size are in the same aligned range, don't add the offset alignment
			+ ( isFullSizeMap
				? ashes::WholeSize
				: ashes::getAlignedSize( size, align ) );

		if ( mappedSize != ashes::WholeSize
			&& mappedSize != ashes::getAlignedSize( mappedSize, align ) )
		{
			log::error << cuT( "StagedUpload: Invalid mapped size alignment: " )
				<< cuT( "] mappedSize = " ) << mappedSize
				<< cuT( ", align = " ) << align << std::endl;
			CU_Failure( "Invalid mapped size alignment" );
		}

		if ( mappedOffset != 0u
			&& mappedOffset != ashes::getAlignedSize( mappedOffset, align ) )
		{
			log::error << cuT( "StagedUpload: Invalid mapped offset alignment: " )
				<< cuT( "] mappedOffset = " ) << mappedOffset
				<< cuT( ", align = " ) << align << std::endl;
			CU_Failure( "Invalid mapped offset alignment" );
		}

		if ( !isFullSizeMap
			&& mappedOffset + mappedSize > dstBuffer.getSize() )
		{
			log::error << cuT( "StagedUpload: Mapped destination is bigger that buffer [" ) << castor::makeString( dstBuffer.getName() )
				<< cuT( "] size, mappedOffset = " ) << mappedOffset
				<< cuT( ", mappedSize = " ) << mappedSize
				<< cuT( ", buffer size = " ) << dstBuffer.getSize() << std::endl;
			CU_Failure( "Trying to copy more than there can be in dst buffer" );
		}

		size = std::min( dstBuffer.getSize(), size );
		auto dst = dstBuffer.lock( mappedOffset, mappedSize, 0u );

		if ( dst )
		{
			std::memcpy( dst + offsetDiff, data, size );
			dstBuffer.flush( mappedOffset, mappedSize );
			dstBuffer.unlock();
		}

		return dst != nullptr;
	}

	void UploadData::doUploadBuffer( BufferDataRange const & data
		, ashes::BufferBase const * srcBuffer
		, VkDeviceSize srcOffset )const
	{
		auto & dstBuffer = *data.dstBuffer;
		auto dstCurFlags = dstBuffer.getCompatibleStageFlags();
		auto dstTrsFlags = srcBuffer ? VkPipelineStageFlags{ VK_PIPELINE_STAGE_TRANSFER_BIT } : VkPipelineStageFlags{ VK_PIPELINE_STAGE_HOST_BIT };

		if ( dstCurFlags != dstTrsFlags )
		{
			m_commandBuffer->memoryBarrier( dstCurFlags
				, dstTrsFlags
				, dstBuffer.makeMemoryTransitionBarrier( dstTrsFlags ) );
		}

		if ( dstBuffer.getSize() < data.dstOffset + data.srcSize )
		{
			log::error << cuT( "StagedUpload: Trying to copy more than there can be in dst [" ) << castor::makeString( dstBuffer.getName() )
				<< cuT( "] buffer: dstOffset = " ) << data.dstOffset
				<< cuT( ", srcSize = " ) << data.srcSize << std::endl;
			CU_Failure( "Trying to copy more than there can be in dst buffer" );
		}

		if ( srcBuffer )
		{
			traceUpload( cuT( "    Registering buffer upload commands: [" ) << castor::toUtf8( data.dstBuffer->getName() )
				<< cuT( "(" ) << data.dstBuffer->getSize()
				<< cuT( ")], Offset: " ) << data.dstOffset
				<< cuT( ", from buffer [" ) << castor::makeString( srcBuffer->getName() )
				<< cuT( "(" ) << srcBuffer->getSize()
				<< cuT( ")], Offset: " ) << srcOffset
				<< cuT( ", Upload Size: " ) << data.srcSize
				<< std::endl );

			if ( srcBuffer->getSize() < srcOffset + data.srcSize )
			{
				log::error << cuT( "StagedUpload: Trying to copy more than there is in src [" ) << castor::makeString( srcBuffer->getName() )
					<< cuT( "] buffer: srcOffset = " ) << srcOffset
					<< cuT( ", srcSize = " ) << data.srcSize << std::endl;
				CU_Failure( "Trying to copy more than there is in src buffer" );
			}

			m_commandBuffer->copyBuffer( *srcBuffer
				, dstBuffer
				, data.srcSize
				, srcOffset
				, data.dstOffset );
		}
		else
		{
			traceUpload( cuT( "    Registering buffer upload commands: [" ) << castor::makeString( data.dstBuffer->getName() )
				<< cuT( "(" ) << data.dstBuffer->getSize()
				<< cuT( ")], Offset: " ) << data.dstOffset
				<< cuT( ", Upload Size: " ) << data.srcSize
				<< std::endl );

			doCopyData( data.srcData
				, data.srcSize
				, dstBuffer
				, data.dstOffset );
		}

		if ( dstTrsFlags != data.dstPipelineFlags )
		{
			m_commandBuffer->memoryBarrier( dstTrsFlags
				, data.dstPipelineFlags
				, dstBuffer.makeMemoryTransitionBarrier( data.dstAccessFlags ) );
		}
	}

	void UploadData::doUploadImage( ImageDataRange & data
		, ashes::BufferBase const & srcBuffer
		, VkDeviceSize srcOffset )const
	{
		traceUpload( cuT( "    Registering image upload commands: [" ) << castor::makeString( data.dstImage->getName() )
			<< cuT( "], Layout: [" ) << data.dstLayout
			<< cuT( "], Range: [" ) << data.dstRange
			<< cuT( ", from buffer [" ) << castor::makeString( srcBuffer.getName() )
			<< cuT( "(" ) << srcBuffer.getSize()
			<< cuT( ")], Offset: " ) << srcOffset
			<< cuT( ", Upload Size: " ) << data.srcSize
			<< std::endl );
		bool is3D = data.dstLayout.type == castor::ImageLayout::e3D;
		auto & dstImage = *data.dstImage;

		ashes::VkBufferImageCopyArray copies;
		VkExtent3D baseDimensions{ dstImage.getDimensions().width
			, dstImage.getDimensions().height
			, std::max( dstImage.getLayerCount(), dstImage.getDimensions().depth ) };

		for ( auto layer = data.dstRange.baseArrayLayer;
			layer < data.dstRange.baseArrayLayer + data.dstRange.layerCount;
			++layer )
		{
			VkImageSubresourceLayers subresourceLayers{ data.dstRange.aspectMask
				, 0u
				, ( is3D ? 0u : layer )
				, 1u };

			for ( auto level = data.dstRange.baseMipLevel;
				level < data.dstRange.baseMipLevel + data.dstRange.levelCount;
				++level )
			{
				subresourceLayers.mipLevel = level;
				copies.push_back( { srcOffset + data.dstLayout.layerMipOffset( layer - data.dstRange.baseArrayLayer, level )
					, 0u
					, 0u
					, subresourceLayers
					, VkOffset3D{ 0
						, 0
						, int32_t( is3D ? std::max( 1u, layer >> level ) : 0u ) }
					, VkExtent3D{ std::max( 1u, baseDimensions.width >> level )
						, std::max( 1u, baseDimensions.height >> level )
						, 1u } } );
			}
		}

		if ( is3D )
		{
			data.dstRange.layerCount = 1u;
		}

		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, dstImage.makeTransition( VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, data.dstRange ) );
		m_commandBuffer->copyToImage( copies, srcBuffer, dstImage );
		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, data.dstPipelineFlags
			, dstImage.makeTransition( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, data.dstImageLayout
				, data.dstRange ) );
	}
}
