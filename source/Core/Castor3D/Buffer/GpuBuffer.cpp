#include "Castor3D/Buffer/GpuBuffer.hpp"

#include "Castor3D/Render/Buffer.hpp"
#include "Castor3D/Buffer/GpuBufferBuddyAllocator.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Buffer/UploadData.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <RenderGraph/ResourceHandler.hpp>

#include <ashespp/Command/CommandBuffer.hpp>

CU_ImplementSmartPtr( c3d, GpuBuddyBuffer )
CU_ImplementSmartPtr( c3d, GpuBufferBase )

namespace c3d
{
	//*********************************************************************************************
	
	Pair< DeviceSize, DeviceSize > adaptRange( DeviceSize offset
		, DeviceSize size
		, DeviceSize align )
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
		, Vector< VkBufferCopy > const & regions
		, AccessFlags dstAccessFlags
		, PipelineStageFlags dstPipelineFlags )
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
			, getPipelineStageFlags( dstPipelineFlags )
			, dst.makeMemoryTransitionBarrier( getAccessFlags( dstAccessFlags ) ) );
	}

	void updateBuffer( ashes::CommandBuffer const & commandBuffer
		, ByteArray src
		, ashes::BufferBase const & dst
		, Vector< VkBufferCopy > const & regions
		, AccessFlags dstAccessFlags
		, PipelineStageFlags dstPipelineFlags )
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
			, getPipelineStageFlags( dstPipelineFlags )
			, dst.makeMemoryTransitionBarrier( getAccessFlags( dstAccessFlags ) ) );
	}

	//*********************************************************************************************

	GpuBufferBase::GpuBufferBase( RenderSystem const & renderSystem
		, crg::ResourcesCache & resources
		, BufferUsageFlags usage
		, MemoryPropertyFlags memoryFlags
		, String const & debugName
		, ashes::QueueShare sharingMode
		, DeviceSize allocatedSize )
		: m_renderSystem{ renderSystem }
		, m_resources{ resources }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ c3d::move( sharingMode ) }
		, m_allocatedSize{ allocatedSize }
		, m_buffer{ makeBufferBase( renderSystem.getRenderDevice(), m_resources
			, uint32_t( m_allocatedSize )
			, m_usage | BufferUsageFlags::eTransferDst
			, m_memoryFlags
			, debugName ) }
		, m_ownData( size_t( m_allocatedSize ) )
		, m_data{ makeArrayView( m_ownData.begin()
			, m_ownData.end() ) }
	{
	}

	GpuBufferBase::~GpuBufferBase()noexcept
	{
		m_buffer->destroy();
	}

	void GpuBufferBase::upload( UploadData & uploader )
	{
		HashMap< size_t, MemoryRangeArray > allRanges;
		c3d::swap( m_ranges, allRanges );

		for ( auto const & [id, ranges] : allRanges )
		{
			for ( auto const & range : ranges )
			{
				upload( uploader
					, range.offset, range.size
					, range.dstAccessState );
			}
		}
	}

	void GpuBufferBase::upload( UploadData & staging
		, DeviceSize offset
		, DeviceSize size
		, AccessState dstAccessState )
	{
		auto [o, s] = adaptRange( offset
			, size
			, m_renderSystem.getValue( GpuMin::eBufferMapSize ) );
		staging.pushUpload( m_ownData.data() + o, s
			, *getBuffer().buffer, o
			, dstAccessState );
	}

	void GpuBufferBase::markDirty( DeviceSize offset
		, DeviceSize size
		, AccessState dstAccessState )
	{
		auto hash = std::hash< int32_t >{}( int32_t( dstAccessState.access ) );
		hash = hashCombine( hash, int32_t( dstAccessState.pipelineStage ) );
		auto & ranges = m_ranges.try_emplace( hash ).first->second;
		auto it = std::find_if( ranges.begin()
			, ranges.end()
			, [offset]( MemoryRange const & lookup )
			{
				return lookup.offset == offset;
			} );

		if ( it == ranges.end() )
		{
			ranges.emplace_back( offset, size, std::move( dstAccessState ) );
		}
	}

	crg::BufferViewId GpuBufferBase::getSubView( DeviceSize offset, DeviceSize size )const
	{
		return m_buffer->getSubView( offset, size );
	}

	//*********************************************************************************************
}
