#include "Castor3D/Buffer/GpuBuffer.hpp"

#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************
	
	std::pair< VkDeviceSize, VkDeviceSize > adaptRange( VkDeviceSize offset
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
		, std::vector< VkBufferCopy > const & regions
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
		, std::vector< VkBufferCopy > const & regions
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
		, castor::String debugName
		, ashes::QueueShare sharingMode
		, VkDeviceSize allocatedSize
		, bool smallData )
		: m_renderSystem{ renderSystem }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ std::move( sharingMode ) }
		, m_allocatedSize{ allocatedSize }
		, m_buffer{ makeBuffer< uint8_t >( renderSystem.getRenderDevice()
			, uint32_t( m_allocatedSize )
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_memoryFlags
			, debugName
			, m_sharingMode ) }
	{
		auto & device = renderSystem.getRenderDevice();

		if ( smallData )
		{
			m_ownData.resize( m_allocatedSize );
			m_data = castor::makeArrayView( m_ownData.data()
				, m_ownData.size() );
		}
		else
		{
			m_stagingBuffer = std::make_unique< ashes::StagingBuffer >( *device
				, debugName + "Staging"
				, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
				, m_allocatedSize );
			m_data = castor::makeArrayView( m_stagingBuffer->getBuffer().lock( 0u
				, m_allocatedSize
				, 0u )
				, m_allocatedSize );
		}
	}

	void GpuBufferBase::upload( ashes::CommandBuffer const & commandBuffer )
	{
		std::unordered_map< size_t, MemoryRangeArray > ranges;
		std::swap( m_ranges, ranges );

		if ( !ranges.empty() )
		{
			if ( !m_ownData.empty() )
			{
				for ( auto & rangesIt : ranges )
				{
					if ( !rangesIt.second.empty() )
					{
						std::vector< VkBufferCopy > regions;

						for ( auto & range : rangesIt.second )
						{
							auto [offset, size] = adaptRange( range.offset
								, range.size
								, 4u );
							regions.push_back( { offset, offset, size } );
						}

						updateBuffer( commandBuffer
							, m_ownData
							, getBuffer().getBuffer()
							, regions
							, rangesIt.second.front().dstAccessFlags
							, rangesIt.second.front().dstPipelineFlags );
					}
				}
			}
			else if ( m_stagingBuffer )
			{
				auto stgSrcStage = m_stagingBuffer->getBuffer().getCompatibleStageFlags();
				commandBuffer.memoryBarrier( stgSrcStage
					, VK_PIPELINE_STAGE_TRANSFER_BIT
					, m_stagingBuffer->getBuffer().makeTransferSource() );

				for ( auto & rangesIt : ranges )
				{
					if ( !rangesIt.second.empty() )
					{
						std::vector< VkBufferCopy > regions;

						for ( auto & range : rangesIt.second )
						{
							auto [offset, size] = adaptRange( range.offset
								, range.size
								, m_renderSystem.getValue( GpuMin::eBufferMapSize ) );
							regions.push_back( { offset, offset, size } );
						}

						copyBuffer( commandBuffer
							, m_stagingBuffer->getBuffer()
							, getBuffer().getBuffer()
							, regions
							, rangesIt.second.front().dstAccessFlags
							, rangesIt.second.front().dstPipelineFlags );
					}
				}

				auto stgDstStage = m_stagingBuffer->getBuffer().getCompatibleStageFlags();
				commandBuffer.memoryBarrier( stgDstStage
					, VK_PIPELINE_STAGE_HOST_BIT
					, m_stagingBuffer->getBuffer().makeHostWrite() );
			}
		}
	}

	void GpuBufferBase::markDirty( VkDeviceSize offset
		, VkDeviceSize size
		, VkAccessFlags dstAccessFlags
		, VkPipelineStageFlags dstPipelineFlags )
	{
		auto hash = std::hash< int32_t >{}( int32_t( dstAccessFlags ) );
		hash = castor::hashCombine( hash, int32_t( dstPipelineFlags ) );
		auto & ranges = m_ranges.emplace( hash, MemoryRangeArray{} ).first->second;
		auto it = std::find_if( ranges.begin()
			, ranges.end()
			, [offset]( MemoryRange const & lookup )
			{
				return lookup.offset == offset;
			} );

		if ( it == ranges.end() )
		{
			ranges.push_back( MemoryRange{ offset, size, dstAccessFlags, dstPipelineFlags } );

			if ( m_stagingBuffer )
			{
				auto [o, s] = adaptRange( offset, size, m_renderSystem.getValue( GpuMin::eBufferMapSize ) );
				m_stagingBuffer->getBuffer().flush( o, s );
			}
		}
	}

	//*********************************************************************************************
}
