#include "Castor3D/Buffer/GpuBuffer.hpp"

#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Command/CommandBuffer.hpp>

CU_ImplementSmartPtr( castor3d, GpuBuddyBuffer )
CU_ImplementSmartPtr( castor3d, GpuBufferBase )

namespace castor3d
{
	//*********************************************************************************************
	
	castor::Pair< VkDeviceSize, VkDeviceSize > adaptRange( VkDeviceSize offset
		, VkDeviceSize size
		, VkDeviceSize align )
	{
		auto newOffset = ashes::getAlignedSize( offset, align );

		if ( newOffset != offset )
		{
			CU_Require( newOffset >= align );
			newOffset -= align;
			size += offset - newOffset;
		}

		return { newOffset, ashes::getAlignedSize( size, align ) };
	}

	//*********************************************************************************************

	void copyBuffer( ashes::CommandBuffer const & commandBuffer
		, ashes::BufferBase const & src
		, ashes::BufferBase const & dst
		, castor::Vector< VkBufferCopy > const & regions
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags )
	{
		auto dstSrcStage = dst.getCompatibleStageFlags();
		commandBuffer.memoryBarrier( dstSrcStage
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, dst.makeTransferDestination() );
		commandBuffer.copyBuffer( regions
			, src
			, dst );
		auto dstDstStage = dst.getCompatibleStageFlags();
		commandBuffer.memoryBarrier( dstDstStage
			, dstPipelineFlags
			, dst.makeMemoryTransitionBarrier( dstAccessFlags ) );
	}

	void updateBuffer( ashes::CommandBuffer const & commandBuffer
		, castor::ByteArray src
		, ashes::BufferBase const & dst
		, castor::Vector< VkBufferCopy > const & regions
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags )
	{
		auto dstSrcStage = dst.getCompatibleStageFlags();
		commandBuffer.memoryBarrier( dstSrcStage
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, dst.makeTransferDestination() );

		for ( auto & region : regions )
		{
			commandBuffer.updateBuffer( dst
				, region.dstOffset
				, ashes::makeArrayView( src.data() + region.srcOffset, region.size ) );
		}

		auto dstDstStage = dst.getCompatibleStageFlags();
		commandBuffer.memoryBarrier( dstDstStage
			, dstPipelineFlags
			, dst.makeMemoryTransitionBarrier( dstAccessFlags ) );
	}

	//*********************************************************************************************

	GpuBufferBase::GpuBufferBase( RenderSystem const & renderSystem
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, castor::String const & debugName
		, ashes::QueueShare sharingMode
		, VkDeviceSize allocatedSize )
		: m_renderSystem{ renderSystem }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ castor::move( sharingMode ) }
		, m_allocatedSize{ allocatedSize }
		, m_buffer{ makeBuffer< uint8_t >( renderSystem.getRenderDevice()
			, uint32_t( m_allocatedSize )
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_memoryFlags
			, debugName
			, m_sharingMode ) }
		, m_ownData( size_t( m_allocatedSize ) )
		, m_data{ castor::makeArrayView( m_ownData.begin()
			, m_ownData.end() ) }
	{
	}

	void GpuBufferBase::upload( UploadData & uploader )
	{
		castor::UnorderedMap< size_t, MemoryRangeArray > allRanges;
		castor::swap( m_ranges, allRanges );

		for ( auto const & [id, ranges] : allRanges )
		{
			for ( auto const & range : ranges )
			{
				upload( uploader
					, range.offset
					, range.size
					, range.dstAccessFlags
					, range.dstPipelineFlags );
			}
		}
	}

	void GpuBufferBase::upload( UploadData & staging
		, VkDeviceSize offset
		, VkDeviceSize size
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags )
	{
		auto [o, s] = adaptRange( offset
			, size
			, m_renderSystem.getValue( GpuMin::eBufferMapSize ) );
		staging.pushUpload( m_ownData.data() + o
			, s
			, getBuffer().getBuffer()
			, o
			, dstAccessFlags
			, dstPipelineFlags );
	}

	void GpuBufferBase::markDirty( VkDeviceSize offset
		, VkDeviceSize size
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags )
	{
		auto hash = std::hash< int32_t >{}( int32_t( dstAccessFlags ) );
		hash = castor::hashCombine( hash, int32_t( dstPipelineFlags ) );
		auto & ranges = m_ranges.try_emplace( hash ).first->second;
		auto it = std::find_if( ranges.begin()
			, ranges.end()
			, [offset]( MemoryRange const & lookup )
			{
				return lookup.offset == offset;
			} );

		if ( it == ranges.end() )
		{
			ranges.emplace_back( offset, size, dstAccessFlags, dstPipelineFlags );
		}
	}

	//*********************************************************************************************
}
