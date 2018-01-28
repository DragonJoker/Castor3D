/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Command/VkCommandBuffer.hpp"

#include "Buffer/VkBuffer.hpp"
#include "Buffer/VkGeometryBuffers.hpp"
#include "Buffer/VkUniformBuffer.hpp"
#include "Command/VkCommandPool.hpp"
#include "Core/VkDevice.hpp"
#include "Core/VkRenderingResources.hpp"
#include "Descriptor/VkDescriptorSet.hpp"
#include "Descriptor/VkDescriptorSetBinding.hpp"
#include "Image/VkImageSubresourceRange.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"
#include "Miscellaneous/VkBufferCopy.hpp"
#include "Miscellaneous/VkBufferImageCopy.hpp"
#include "Miscellaneous/VkImageCopy.hpp"
#include "Miscellaneous/VkQueryPool.hpp"
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
#include <RenderPass/TextureAttachment.hpp>

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
		auto res = vk::AllocateCommandBuffers( m_device, &cmdAllocInfo, &m_commandBuffer );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "CommandBuffer creation failed: " + getLastError() };
		}
	}

	CommandBuffer::~CommandBuffer()
	{
		vk::FreeCommandBuffers( m_device, m_pool, 1, &m_commandBuffer );
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
		auto res = vk::BeginCommandBuffer( m_commandBuffer, &cmdBufInfo );
		m_currentPipeline = nullptr;
		return checkError( res );
	}

	bool CommandBuffer::begin( renderer::CommandBufferUsageFlags flags
		, renderer::RenderPass const & renderPass
		, uint32_t subpass
		, renderer::FrameBuffer const & frameBuffer
		, bool occlusionQueryEnable
		, renderer::QueryControlFlags queryFlags
		, renderer::QueryPipelineStatisticFlags pipelineStatistics )const
	{
		m_inheritanceInfo = VkCommandBufferInheritanceInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,  // sType;
			nullptr,                                            // pNext;
			static_cast< RenderPass const & >( renderPass ),    // renderPass;
			subpass,                                            // subpass;
			static_cast< FrameBuffer const & >( frameBuffer ),  // framebuffer;
			occlusionQueryEnable,                               // occlusionQueryEnable;
			convert( queryFlags ),                              // queryFlags;
			convert( pipelineStatistics )                       // pipelineStatistics;
		};
		VkCommandBufferBeginInfo cmdBufInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			convert( flags ),                            // flags
			&m_inheritanceInfo                           // pInheritanceInfo
		};
		DEBUG_DUMP( cmdBufInfo );
		auto res = vk::BeginCommandBuffer( m_commandBuffer, &cmdBufInfo );
		m_currentPipeline = nullptr;
		return checkError( res );
	}

	bool CommandBuffer::end()const
	{
		auto res = vk::EndCommandBuffer( m_commandBuffer );
		m_currentPipeline = nullptr;
		return checkError( res );
	}

	bool CommandBuffer::reset( renderer::CommandBufferResetFlags flags )const
	{
		auto res = vk::ResetCommandBuffer( m_commandBuffer, convert( flags ) );
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
				{                                                 // extent
					uint32_t( vkfbo.getDimensions()[0] ),             // width
					uint32_t( vkfbo.getDimensions()[1] ),             // height
				}
			},
			uint32_t( vkclearValues.size() ),                   // clearValueCount
			vkclearValues.data()                                // pClearValues
		};
		DEBUG_DUMP( beginInfo );
		vk::CmdBeginRenderPass( m_commandBuffer
			, &beginInfo
			, convert( contents ) );
	}

	void CommandBuffer::nextSubpass( renderer::SubpassContents contents )const
	{
		vk::CmdNextSubpass( m_commandBuffer, convert( contents ) );
	}

	void CommandBuffer::endRenderPass()const
	{
		vk::CmdEndRenderPass( m_commandBuffer );
	}

	void CommandBuffer::executeCommands( renderer::CommandBufferCRefArray const & commands )const
	{
		auto vkCommands = makeVkArray< VkCommandBuffer >( convert( commands ) );
		vk::CmdExecuteCommands( m_commandBuffer
			, uint32_t( vkCommands.size() )
			, vkCommands.data() );
	}

	void CommandBuffer::clear( renderer::TextureView const & image
		, renderer::RgbaColour const & colour )const
	{
		auto vkcolour = convert( colour );
		auto vksubresourceRange = convert( image.getSubResourceRange() );
		vk::CmdClearColorImage( m_commandBuffer
			, static_cast< Texture const & >( image.getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, &vkcolour
			, 1
			, &vksubresourceRange );
	}

	void CommandBuffer::bindPipeline( renderer::Pipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )const
	{
		vk::CmdBindPipeline( m_commandBuffer
			, convert( bindingPoint )
			, static_cast< Pipeline const & >( pipeline ) );
		m_currentPipeline = &static_cast< Pipeline const & >( pipeline );
	}

	void CommandBuffer::bindGeometryBuffers( renderer::GeometryBuffers const & geometryBuffers )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		std::vector< std::reference_wrapper< Buffer const > > buffers;
		std::vector< uint64_t > offsets;

		for ( auto & vbo : geometryBuffers.getVbos() )
		{
			buffers.emplace_back( static_cast< Buffer const & >( vbo.vbo.getBuffer() ) );
			offsets.emplace_back( vbo.offset );
		}

		vk::CmdBindVertexBuffers( m_commandBuffer
			, 0u
			, uint32_t( buffers.size() )
			, makeVkArray< VkBuffer >( buffers ).data()
			, offsets.data() );

		if ( geometryBuffers.hasIbo() )
		{
			auto & ibo = geometryBuffers.getIbo();
			vk::CmdBindIndexBuffer( m_commandBuffer
				, static_cast< Buffer const & >( ibo.buffer )
				, ibo.offset
				, convert( ibo.type ) );
		}
	}

	void CommandBuffer::memoryBarrier( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::BufferMemoryBarrier const & transitionBarrier )const
	{
		auto vkafter = convert( after );
		auto vkbefore = convert( before );
		auto vktb = convert( transitionBarrier );
		vk::CmdPipelineBarrier( m_commandBuffer
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
		vk::CmdPipelineBarrier( m_commandBuffer
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

	void CommandBuffer::bindDescriptorSet( renderer::DescriptorSet const & descriptorSet
		, renderer::PipelineLayout const & layout
		, renderer::PipelineBindPoint bindingPoint )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		VkDescriptorSet set{ static_cast< DescriptorSet const & >( descriptorSet ) };
		vk::CmdBindDescriptorSets( m_commandBuffer
			, convert( bindingPoint )
			, static_cast< PipelineLayout const & >( layout )
			, 0u
			, 1u
			, &set
			, 0u
			, nullptr );
	}

	void CommandBuffer::setViewport( renderer::Viewport const & viewport )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		auto vkviewport = convert( viewport );
		vk::CmdSetViewport( m_commandBuffer
			, 0u
			, 1u
			, &vkviewport );
	}

	void CommandBuffer::setScissor( renderer::Scissor const & scissor )const
	{
		assert( m_currentPipeline && "No pipeline bound." );
		auto vkscissor = convert( scissor );
		vk::CmdSetScissor( m_commandBuffer
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
		vk::CmdDraw( m_commandBuffer
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
		vk::CmdDrawIndexed( m_commandBuffer
			, indexCount
			, instCount
			, firstIndex
			, vertexOffset
			, firstInstance );
	}

	void CommandBuffer::copyToImage( renderer::BufferImageCopy const & copyInfo
		, renderer::BufferBase const & src
		, renderer::TextureView const & dst )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		vk::CmdCopyBufferToImage( m_commandBuffer
			, static_cast< Buffer const & >( src )
			, static_cast< Texture const & >( dst.getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, 1
			, &vkcopyInfo );
	}

	void CommandBuffer::copyToBuffer( renderer::BufferImageCopy const & copyInfo
		, renderer::TextureView const & src
		, renderer::BufferBase const & dst )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		vk::CmdCopyImageToBuffer( m_commandBuffer
			, static_cast< Texture const & >( src.getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, static_cast< Buffer const & >( dst )
			, 1
			, &vkcopyInfo );
	}

	void CommandBuffer::copyBuffer( renderer::BufferCopy const & copyInfo
		, renderer::BufferBase const & src
		, renderer::BufferBase const & dst )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		vk::CmdCopyBuffer( m_commandBuffer
			, static_cast< Buffer const & >( src )
			, static_cast< Buffer const & >( dst )
			, 1
			, &vkcopyInfo );
	}

	void CommandBuffer::copyImage( renderer::ImageCopy const & copyInfo
		, renderer::TextureView const & src
		, renderer::TextureView const & dst )const
	{
		auto vkcopyInfo = convert( copyInfo );
		DEBUG_DUMP( vkcopyInfo );
		vk::CmdCopyImage( m_commandBuffer
			, static_cast< Texture const & >( src.getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, static_cast< Texture const & >( dst.getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, 1
			, &vkcopyInfo );
	}

	void CommandBuffer::blitImage( renderer::ImageBlit const & blit
		, renderer::TextureAttachment const & src
		, renderer::TextureAttachment const & dst
		, renderer::Filter filter )const
	{
		auto vkblitInfo = convert( blit );
		DEBUG_DUMP( vkblitInfo );
		vk::CmdBlitImage( m_commandBuffer
			, static_cast< Texture const & >( static_cast< renderer::TextureAttachment const & >( src ).getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, static_cast< Texture const & >( static_cast< renderer::TextureAttachment const & >( dst ).getTexture() )
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, 1
			, &vkblitInfo
			, convert( filter ) );
	}

	void CommandBuffer::resetQueryPool( renderer::QueryPool const & pool
		, uint32_t firstQuery
		, uint32_t queryCount )const
	{
		vk::CmdResetQueryPool( m_commandBuffer
			, static_cast< QueryPool const & >( pool )
			, firstQuery
			, queryCount );
	}

	void CommandBuffer::beginQuery( renderer::QueryPool const & pool
		, uint32_t query
		, renderer::QueryControlFlags flags )const
	{
		vk::CmdBeginQuery( m_commandBuffer
			, static_cast< QueryPool const & >( pool )
			, query
			, convert( flags ) );
	}

	void CommandBuffer::endQuery( renderer::QueryPool const & pool
		, uint32_t query )const
	{
		vk::CmdEndQuery( m_commandBuffer
			, static_cast< QueryPool const & >( pool )
			, query );
	}

	void CommandBuffer::writeTimestamp( renderer::PipelineStageFlag pipelineStage
		, renderer::QueryPool const & pool
		, uint32_t query )const
	{
		vk::CmdWriteTimestamp( m_commandBuffer
			, convert( pipelineStage )
			, static_cast< QueryPool const & >( pool )
			, query );
	}

	void CommandBuffer::pushConstants( renderer::PipelineLayout const & layout
		, renderer::PushConstantsBuffer const & pcb )const
	{
		vk::CmdPushConstants( m_commandBuffer
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
		vk::CmdDispatch( m_commandBuffer
			, groupCountX
			, groupCountY
			, groupCountZ );
	}
}
