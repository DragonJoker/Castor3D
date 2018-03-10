/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Command/VkCommandBuffer.hpp"

#include "Buffer/VkBuffer.hpp"
#include "Buffer/VkUniformBuffer.hpp"
#include "Command/VkCommandPool.hpp"
#include "Core/VkDevice.hpp"
#include "Core/VkRenderingResources.hpp"
#include "Descriptor/VkDescriptorSet.hpp"
#include "Image/VkImageSubresourceRange.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"
#include "Miscellaneous/VkBufferCopy.hpp"
#include "Miscellaneous/VkBufferImageCopy.hpp"
#include "Miscellaneous/VkImageCopy.hpp"
#include "Miscellaneous/VkQueryPool.hpp"
#include "Pipeline/VkComputePipeline.hpp"
#include "Pipeline/VkPipeline.hpp"
#include "Pipeline/VkPipelineLayout.hpp"
#include "Pipeline/VkScissor.hpp"
#include "Pipeline/VkViewport.hpp"
#include "RenderPass/VkClearValue.hpp"
#include "RenderPass/VkFrameBuffer.hpp"
#include "RenderPass/VkRenderPass.hpp"
#include "Sync/VkBufferMemoryBarrier.hpp"
#include "Sync/VkImageMemoryBarrier.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Buffer/StagingBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

namespace vk_renderer
{
	namespace
	{
		CommandBufferCRefArray convert( renderer::CommandBufferCRefArray const & commands )
		{
			CommandBufferCRefArray result;

			for ( auto & command : commands )
			{
				result.emplace_back( static_cast< CommandBuffer const & >( command.get() ) );
			}

			return result;
		}

		DescriptorSetCRefArray convert( renderer::DescriptorSetCRefArray const & descriptors )
		{
			DescriptorSetCRefArray result;

			for ( auto & descriptor : descriptors )
			{
				result.emplace_back( static_cast< DescriptorSet const & >( descriptor.get() ) );
			}

			return result;
		}

		std::vector< VkBufferImageCopy > convert( renderer::BufferImageCopyArray const & copies )
		{
			std::vector< VkBufferImageCopy > result;

			for ( auto & copy : copies )
			{
				result.emplace_back( vk_renderer::convert( copy ) );
			}

			return result;
		}
	}

	CommandBuffer::CommandBuffer( Device const & device
		, CommandPool const & pool
		, bool primary )
		: m_device{ device }
		, m_pool{ pool }
	{
		VkCommandBufferAllocateInfo cmdAllocInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			m_pool,                                   // commandPool
			primary                                   // level
				? VK_COMMAND_BUFFER_LEVEL_PRIMARY
				: VK_COMMAND_BUFFER_LEVEL_SECONDARY,
			1                                         // commandBufferCount
		};
		DEBUG_DUMP( cmdAllocInfo );
		auto res = m_device.vkAllocateCommandBuffers( m_device, &cmdAllocInfo, &m_commandBuffer );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "CommandBuffer creation failed: " + getLastError() };
		}
	}

	CommandBuffer::~CommandBuffer()
	{
		m_device.vkFreeCommandBuffers( m_device, m_pool, 1, &m_commandBuffer );
	}

	bool CommandBuffer::begin( renderer::CommandBufferUsageFlags flags )const
	{
		VkCommandBufferBeginInfo cmdBufInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			convert( flags ),                            // flags
			nullptr                                      // pInheritanceInfo
		};
		DEBUG_DUMP( cmdBufInfo );
		auto res = m_device.vkBeginCommandBuffer( m_commandBuffer, &cmdBufInfo );
		m_currentPipeline = nullptr;
		m_currentComputePipeline = nullptr;
		return checkError( res );
	}

	bool CommandBuffer::begin( renderer::CommandBufferUsageFlags flags
		, renderer::CommandBufferInheritanceInfo const & inheritanceInfo )const
	{
		m_inheritanceInfo = convert( inheritanceInfo );
		VkCommandBufferBeginInfo cmdBufInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			convert( flags ),                            // flags
			&m_inheritanceInfo                           // pInheritanceInfo
		};
		DEBUG_DUMP( cmdBufInfo );
		auto res = m_device.vkBeginCommandBuffer( m_commandBuffer, &cmdBufInfo );
		m_currentPipeline = nullptr;
		m_currentComputePipeline = nullptr;
		return checkError( res );
	}

	bool CommandBuffer::end()const
	{
		auto res = m_device.vkEndCommandBuffer( m_commandBuffer );
		m_currentPipeline = nullptr;
		m_currentComputePipeline = nullptr;
		return checkError( res );
	}

	bool CommandBuffer::reset( renderer::CommandBufferResetFlags flags )const
	{
		auto res = m_device.vkResetCommandBuffer( m_commandBuffer, convert( flags ) );
		return checkError( res );
	}

	void CommandBuffer::beginRenderPass( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, renderer::ClearValueArray const & clearValues
		, renderer::SubpassContents contents )const
	{
		auto & vkclearValues = static_cast< RenderPass const & >( renderPass ).getClearValues( clearValues );
		//auto vkclearValues = convert< VkClearValue >( clearValues );
		auto & vkfbo = static_cast< FrameBuffer const & >( frameBuffer );
		VkRenderPassBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			static_cast< RenderPass const & >( renderPass ),    // renderPass
			vkfbo,                                              // framebuffer
			{                                                   // renderArea
				{                                                 // offset
					0,                                                // x
					0                                                 // y
				},
				convert( vkfbo.getDimensions() ),               // extent
			},
			uint32_t( vkclearValues.size() ),                   // clearValueCount
			vkclearValues.data()                                // pClearValues
		};
		DEBUG_DUMP( beginInfo );
		m_device.vkCmdBeginRenderPass( m_commandBuffer
			, &beginInfo
			, convert( contents ) );
	}

	void CommandBuffer::nextSubpass( renderer::SubpassContents contents )const
	{
		m_device.vkCmdNextSubpass( m_commandBuffer, convert( contents ) );
	}

	void CommandBuffer::endRenderPass()const
	{
		m_device.vkCmdEndRenderPass( m_commandBuffer );
	}

	void CommandBuffer::executeCommands( renderer::CommandBufferCRefArray const & commands )const
	{
		auto vkCommands = makeVkArray< VkCommandBuffer >( convert( commands ) );
		m_device.vkCmdExecuteCommands( m_commandBuffer
			, uint32_t( vkCommands.size() )
			, vkCommands.data() );
	}

	void CommandBuffer::clear( renderer::TextureView const & image
		, renderer::RgbaColour const & colour )const
	{
		auto vkcolour = convert( colour );
		auto vksubresourceRange = convert( image.getSubResourceRange() );
		m_device.vkCmdClearColorImage( m_commandBuffer
			, static_cast< Texture const & >( image.getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, &vkcolour
			, 1
			, &vksubresourceRange );
	}

	void CommandBuffer::clear( renderer::TextureView const & image
		, renderer::DepthStencilClearValue const & value )const
	{
		auto vkclear = convert( value );
		auto vksubresourceRange = convert( image.getSubResourceRange() );
		m_device.vkCmdClearDepthStencilImage( m_commandBuffer
			, static_cast< Texture const & >( image.getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, &vkclear
			, 1
			, &vksubresourceRange );
	}

	void CommandBuffer::clearAttachments( renderer::ClearAttachmentArray const & clearAttachments
		, renderer::ClearRectArray const & clearRects )
	{
		auto vkAttaches = convert< VkClearAttachment >( clearAttachments );
		auto vkRects = convert< VkClearRect >( clearRects );
		m_device.vkCmdClearAttachments( m_commandBuffer
			, uint32_t( vkAttaches.size() )
			, vkAttaches.data()
			, uint32_t( vkRects.size() )
			, vkRects.data() );
	}

	void CommandBuffer::bindPipeline( renderer::Pipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )const
	{
		m_device.vkCmdBindPipeline( m_commandBuffer
			, convert( bindingPoint )
			, static_cast< Pipeline const & >( pipeline ) );
		m_currentPipeline = &static_cast< Pipeline const & >( pipeline );
	}

	void CommandBuffer::bindPipeline( renderer::ComputePipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )const
	{
		m_device.vkCmdBindPipeline( m_commandBuffer
			, convert( bindingPoint )
			, static_cast< ComputePipeline const & >( pipeline ) );
		m_currentComputePipeline = &static_cast< ComputePipeline const & >( pipeline );
	}

	void CommandBuffer::bindVertexBuffers( uint32_t firstBinding
		, renderer::BufferCRefArray const & buffers
		, renderer::UInt64Array offsets )const
	{
		std::vector< std::reference_wrapper< Buffer const > > vkbuffers;

		for ( auto & buffer : buffers )
		{
			vkbuffers.emplace_back( static_cast< Buffer const & >( buffer.get() ) );
		}

		m_device.vkCmdBindVertexBuffers( m_commandBuffer
			, firstBinding
			, uint32_t( vkbuffers.size() )
			, makeVkArray< VkBuffer >( vkbuffers ).data()
			, offsets.data() );
	}

	void CommandBuffer::bindIndexBuffer( renderer::BufferBase const & buffer
		, uint64_t offset
		, renderer::IndexType indexType )const
	{
		m_device.vkCmdBindIndexBuffer( m_commandBuffer
			, static_cast< Buffer const & >( buffer )
			, offset
			, convert( indexType ) );
	}

	void CommandBuffer::memoryBarrier( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::BufferMemoryBarrier const & transitionBarrier )const
	{
		auto vkafter = convert( after );
		auto vkbefore = convert( before );
		auto vktb = convert( transitionBarrier );
		m_device.vkCmdPipelineBarrier( m_commandBuffer
			, vkbefore
			, vkafter
			, 0
			, 0u
			, nullptr
			, 1u
			, &vktb
			, 0u
			, nullptr );
	}

	void CommandBuffer::memoryBarrier( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::ImageMemoryBarrier const & transitionBarrier )const
	{
		auto vkafter = convert( after );
		auto vkbefore = convert( before );
		auto vktb = convert( transitionBarrier );
		m_device.vkCmdPipelineBarrier( m_commandBuffer
			, vkbefore
			, vkafter
			, 0
			, 0u
			, nullptr
			, 0u
			, nullptr
			, 1u
			, &vktb );
	}

	void CommandBuffer::bindDescriptorSets( renderer::DescriptorSetCRefArray const & descriptorSets
		, renderer::PipelineLayout const & layout
		, renderer::UInt32Array const & dynamicOffsets
		, renderer::PipelineBindPoint bindingPoint )const
	{
		auto vkDescriptors = makeVkArray< VkDescriptorSet >( convert( descriptorSets ) );
		m_device.vkCmdBindDescriptorSets( m_commandBuffer
			, convert( bindingPoint )
			, static_cast< PipelineLayout const & >( layout )
			, descriptorSets.begin()->get().getBindingPoint()
			, uint32_t( descriptorSets.size() )
			, vkDescriptors.data()
			, uint32_t( dynamicOffsets.size() )
			, dynamicOffsets.data() );
	}

	void CommandBuffer::setViewport( renderer::Viewport const & viewport )const
	{
		auto vkviewport = convert( viewport );
		m_device.vkCmdSetViewport( m_commandBuffer
			, 0u
			, 1u
			, &vkviewport );
	}

	void CommandBuffer::setScissor( renderer::Scissor const & scissor )const
	{
		auto vkscissor = convert( scissor );
		m_device.vkCmdSetScissor( m_commandBuffer
			, 0u
			, 1u
			, &vkscissor );
	}

	void CommandBuffer::draw( uint32_t vtxCount
		, uint32_t instCount
		, uint32_t firstVertex
		, uint32_t firstInstance )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		m_device.vkCmdDraw( m_commandBuffer
			, vtxCount
			, instCount
			, firstVertex
			, firstInstance );
	}

	void CommandBuffer::drawIndexed( uint32_t indexCount
		, uint32_t instCount
		, uint32_t firstIndex
		, uint32_t vertexOffset
		, uint32_t firstInstance )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		m_device.vkCmdDrawIndexed( m_commandBuffer
			, indexCount
			, instCount
			, firstIndex
			, vertexOffset
			, firstInstance );
	}

	void CommandBuffer::drawIndirect( renderer::BufferBase const & buffer
		, uint32_t offset
		, uint32_t drawCount
		, uint32_t stride )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		m_device.vkCmdDrawIndirect( m_commandBuffer
			, static_cast< Buffer const & >( buffer )
			, offset
			, drawCount
			, stride );
	}

	void CommandBuffer::drawIndexedIndirect( renderer::BufferBase const & buffer
		, uint32_t offset
		, uint32_t drawCount
		, uint32_t stride )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		m_device.vkCmdDrawIndexedIndirect( m_commandBuffer
			, static_cast< Buffer const & >( buffer )
			, offset
			, drawCount
			, stride );
	}

	void CommandBuffer::copyToImage( renderer::BufferImageCopyArray const & copyInfo
		, renderer::BufferBase const & src
		, renderer::Texture const & dst )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		m_device.vkCmdCopyBufferToImage( m_commandBuffer
			, static_cast< Buffer const & >( src )
			, static_cast< Texture const & >( dst )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, uint32_t( vkcopyInfo.size() )
			, vkcopyInfo.data() );
	}

	void CommandBuffer::copyToBuffer( renderer::BufferImageCopyArray const & copyInfo
		, renderer::Texture const & src
		, renderer::BufferBase const & dst )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		m_device.vkCmdCopyImageToBuffer( m_commandBuffer
			, static_cast< Texture const & >( src )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, static_cast< Buffer const & >( dst )
			, uint32_t( vkcopyInfo.size() )
			, vkcopyInfo.data() );
	}

	void CommandBuffer::copyBuffer( renderer::BufferCopy const & copyInfo
		, renderer::BufferBase const & src
		, renderer::BufferBase const & dst )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		m_device.vkCmdCopyBuffer( m_commandBuffer
			, static_cast< Buffer const & >( src )
			, static_cast< Buffer const & >( dst )
			, 1
			, &vkcopyInfo );
	}

	void CommandBuffer::copyImage( renderer::ImageCopy const & copyInfo
		, renderer::Texture const & src
		, renderer::ImageLayout srcLayout
		, renderer::Texture const & dst
		, renderer::ImageLayout dstLayout )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		m_device.vkCmdCopyImage( m_commandBuffer
			, static_cast< Texture const & >( src )
			, convert( srcLayout )
			, static_cast< Texture const & >( dst )
			, convert( dstLayout )
			, 1
			, &vkcopyInfo );
	}

	void CommandBuffer::blitImage( renderer::Texture const & srcImage
		, renderer::ImageLayout srcLayout
		, renderer::Texture const & dstImage
		, renderer::ImageLayout dstLayout
		, std::vector< renderer::ImageBlit > const & regions
		, renderer::Filter filter )const
	{
		auto vkregions = convert< VkImageBlit >( regions );
		DEBUG_DUMP( vkregions );
		m_device.vkCmdBlitImage( m_commandBuffer
			, static_cast< Texture const & >( srcImage )
			, convert( srcLayout )
			, static_cast< Texture const & >( dstImage )
			, convert( dstLayout )
			, uint32_t( vkregions.size() )
			, vkregions.data()
			, convert( filter ) );
	}

	void CommandBuffer::resetQueryPool( renderer::QueryPool const & pool
		, uint32_t firstQuery
		, uint32_t queryCount )const
	{
		m_device.vkCmdResetQueryPool( m_commandBuffer
			, static_cast< QueryPool const & >( pool )
			, firstQuery
			, queryCount );
	}

	void CommandBuffer::beginQuery( renderer::QueryPool const & pool
		, uint32_t query
		, renderer::QueryControlFlags flags )const
	{
		m_device.vkCmdBeginQuery( m_commandBuffer
			, static_cast< QueryPool const & >( pool )
			, query
			, convert( flags ) );
	}

	void CommandBuffer::endQuery( renderer::QueryPool const & pool
		, uint32_t query )const
	{
		m_device.vkCmdEndQuery( m_commandBuffer
			, static_cast< QueryPool const & >( pool )
			, query );
	}

	void CommandBuffer::writeTimestamp( renderer::PipelineStageFlag pipelineStage
		, renderer::QueryPool const & pool
		, uint32_t query )const
	{
		m_device.vkCmdWriteTimestamp( m_commandBuffer
			, convert( pipelineStage )
			, static_cast< QueryPool const & >( pool )
			, query );
	}

	void CommandBuffer::pushConstants( renderer::PipelineLayout const & layout
		, renderer::PushConstantsBufferBase const & pcb )const
	{
		m_device.vkCmdPushConstants( m_commandBuffer
			, static_cast< PipelineLayout const & >( layout )
			, convert( pcb.getStageFlags() )
			, pcb.getOffset()
			, pcb.getSize()
			, pcb.getData() );
	}

	void CommandBuffer::dispatch( uint32_t groupCountX
		, uint32_t groupCountY
		, uint32_t groupCountZ )const
	{
		assert( m_currentComputePipeline && "No compute pipeline bound." );
		m_device.vkCmdDispatch( m_commandBuffer
			, groupCountX
			, groupCountY
			, groupCountZ );
	}

	void CommandBuffer::dispatchIndirect( renderer::BufferBase const & buffer
		, uint32_t offset )const
	{
		assert( m_currentComputePipeline && "No pipeline bound." );
		m_device.vkCmdDispatchIndirect( m_commandBuffer
			, static_cast< Buffer const & >( buffer )
			, offset );
	}

	void CommandBuffer::setLineWidth( float width )const
	{
		m_device.vkCmdSetLineWidth( m_commandBuffer, width );
	}

	void CommandBuffer::setDepthBias( float constantFactor
		, float clamp
		, float slopeFactor )const
	{
		m_device.vkCmdSetDepthBias( m_commandBuffer
			, constantFactor
			, clamp
			, slopeFactor );
	}
}
