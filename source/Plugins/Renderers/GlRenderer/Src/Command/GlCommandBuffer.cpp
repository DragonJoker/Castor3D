/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Command/GlCommandBuffer.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Buffer/GlGeometryBuffers.hpp"
#include "Buffer/GlUniformBuffer.hpp"
#include "Command/GlCommandPool.hpp"
#include "Core/GlDevice.hpp"
#include "Core/GlRenderingResources.hpp"
#include "Descriptor/GlDescriptorSet.hpp"
#include "Image/GlTexture.hpp"
#include "Pipeline/GlComputePipeline.hpp"
#include "Pipeline/GlPipeline.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "Sync/GlBufferMemoryBarrier.hpp"
#include "Sync/GlImageMemoryBarrier.hpp"

#include "Commands/GlBeginQueryCommand.hpp"
#include "Commands/GlBeginRenderPassCommand.hpp"
#include "Commands/GlBindComputePipelineCommand.hpp"
#include "Commands/GlBindDescriptorSetCommand.hpp"
#include "Commands/GlBindGeometryBuffersCommand.hpp"
#include "Commands/GlBindPipelineCommand.hpp"
#include "Commands/GlBlitImageCommand.hpp"
#include "Commands/GlBufferMemoryBarrierCommand.hpp"
#include "Commands/GlClearAttachmentsCommand.hpp"
#include "Commands/GlClearColourCommand.hpp"
#include "Commands/GlClearDepthStencilCommand.hpp"
#include "Commands/GlCopyBufferCommand.hpp"
#include "Commands/GlCopyBufferToImageCommand.hpp"
#include "Commands/GlCopyImageCommand.hpp"
#include "Commands/GlCopyImageToBufferCommand.hpp"
#include "Commands/GlDispatchCommand.hpp"
#include "Commands/GlDispatchIndirectCommand.hpp"
#include "Commands/GlDrawCommand.hpp"
#include "Commands/GlDrawIndexedCommand.hpp"
#include "Commands/GlDrawIndexedIndirectCommand.hpp"
#include "Commands/GlDrawIndirectCommand.hpp"
#include "Commands/GlEndQueryCommand.hpp"
#include "Commands/GlEndRenderPassCommand.hpp"
#include "Commands/GlEndSubpassCommand.hpp"
#include "Commands/GlImageMemoryBarrierCommand.hpp"
#include "Commands/GlNextSubpassCommand.hpp"
#include "Commands/GlPushConstantsCommand.hpp"
#include "Commands/GlResetQueryPoolCommand.hpp"
#include "Commands/GlScissorCommand.hpp"
#include "Commands/GlSetDepthBiasCommand.hpp"
#include "Commands/GlSetLineWidthCommand.hpp"
#include "Commands/GlViewportCommand.hpp"
#include "Commands/GlWriteTimestampCommand.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>

#include <algorithm>

namespace gl_renderer
{
	CommandBuffer::CommandBuffer( Device const & device
		, renderer::CommandPool const & pool
		, bool primary )
		: renderer::CommandBuffer{ device, pool, primary }
		, m_device{ device }
	{
	}

	bool CommandBuffer::begin( renderer::CommandBufferUsageFlags flags )const
	{
		m_commands.clear();
		m_state = State{};
		m_state.m_beginFlags = flags;
		return true;
	}

	bool CommandBuffer::begin( renderer::CommandBufferUsageFlags flags
		, renderer::CommandBufferInheritanceInfo const & inheritanceInfo )const
	{
		m_commands.clear();
		m_state = State{};
		m_state.m_beginFlags = flags;
		return true;
	}

	bool CommandBuffer::end()const
	{
		m_state.m_pushConstantBuffers.clear();
		return true;
	}

	bool CommandBuffer::reset( renderer::CommandBufferResetFlags flags )const
	{
		m_commands.clear();
		return true;
	}

	void CommandBuffer::beginRenderPass( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, renderer::ClearValueArray const & clearValues
		, renderer::SubpassContents contents )const
	{
		m_state.m_currentRenderPass = &static_cast< RenderPass const & >( renderPass );
		m_state.m_currentFrameBuffer = &frameBuffer;
		m_state.m_currentSubpassIndex = 0u;
		m_state.m_currentSubpass = &m_state.m_currentRenderPass->getSubpasses()[m_state.m_currentSubpassIndex++];
		m_commands.emplace_back( std::make_unique< BeginRenderPassCommand >( renderPass
			, frameBuffer
			, clearValues
			, contents
			, *m_state.m_currentSubpass ) );
	}

	void CommandBuffer::nextSubpass( renderer::SubpassContents contents )const
	{
		m_commands.emplace_back( std::make_unique< EndSubpassCommand >( m_device
			, *m_state.m_currentFrameBuffer
			, *m_state.m_currentSubpass ) );
		m_state.m_currentSubpass = &m_state.m_currentRenderPass->getSubpasses()[m_state.m_currentSubpassIndex++];
		m_commands.emplace_back( std::make_unique< NextSubpassCommand >( *m_state.m_currentRenderPass
			, *m_state.m_currentFrameBuffer
			, *m_state.m_currentSubpass ) );
		m_state.m_boundVbos.clear();
	}

	void CommandBuffer::endRenderPass()const
	{
		m_commands.emplace_back( std::make_unique< EndSubpassCommand >( m_device
			, *m_state.m_currentFrameBuffer
			, *m_state.m_currentSubpass ) );
		m_commands.emplace_back( std::make_unique< EndRenderPassCommand >() );
		m_state.m_boundVbos.clear();
	}

	void CommandBuffer::executeCommands( renderer::CommandBufferCRefArray const & commands )const
	{
		for ( auto & commandBuffer : commands )
		{
			static_cast< CommandBuffer const & >( commandBuffer.get() ).initialiseGeometryBuffers();

			for ( auto & command : static_cast< CommandBuffer const & >( commandBuffer.get() ).getCommands() )
			{
				m_commands.emplace_back( command->clone() );
			}
		}
	}

	void CommandBuffer::clear( renderer::TextureView const & image
		, renderer::RgbaColour const & colour )const
	{
		m_commands.emplace_back( std::make_unique< ClearColourCommand >( image, colour ) );
	}

	void CommandBuffer::clear( renderer::TextureView const & image
		, renderer::DepthStencilClearValue const & value )const
	{
		m_commands.emplace_back( std::make_unique< ClearDepthStencilCommand >( image, value ) );
	}

	void CommandBuffer::clearAttachments( renderer::ClearAttachmentArray const & clearAttachments
		, renderer::ClearRectArray const & clearRects )
	{
		m_commands.emplace_back( std::make_unique< ClearAttachmentsCommand >( m_device, clearAttachments, clearRects ) );
	}

	void CommandBuffer::bindPipeline( renderer::Pipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )const
	{
		if ( m_state.m_currentPipeline )
		{
			auto src = m_state.m_currentPipeline->getVertexInputStateHash();
			auto dst = static_cast< Pipeline const & >( pipeline ).getVertexInputStateHash();

			if ( src != dst )
			{
				IboBinding empty{};
				m_state.m_boundIbo.swap( empty );
				m_state.m_boundVbos.clear();
			}
		}

		m_state.m_currentPipeline = &static_cast< Pipeline const & >( pipeline );
		m_commands.emplace_back( std::make_unique< BindPipelineCommand >( m_device, pipeline, bindingPoint ) );

		for ( auto & pcb : m_state.m_pushConstantBuffers )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( *pcb.first
				, *pcb.second ) );
		}

		for ( auto & pcb : m_state.m_currentPipeline->getConstantsPcbs() )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( m_state.m_currentPipeline->getLayout()
				, pcb ) );
		}

		m_state.m_pushConstantBuffers.clear();
	}

	void CommandBuffer::bindPipeline( renderer::ComputePipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )const
	{
		m_state.m_currentComputePipeline = &static_cast< ComputePipeline const & >( pipeline );
		m_commands.emplace_back( std::make_unique< BindComputePipelineCommand >( m_device, pipeline, bindingPoint ) );

		for ( auto & pcb : m_state.m_pushConstantBuffers )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( *pcb.first
				, *pcb.second ) );
		}

		for ( auto & pcb : m_state.m_currentComputePipeline->getConstantsPcbs() )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( m_state.m_currentComputePipeline->getLayout()
				, pcb ) );
		}

		m_state.m_pushConstantBuffers.clear();
	}

	void CommandBuffer::bindVertexBuffers( uint32_t firstBinding
		, renderer::BufferCRefArray const & buffers
		, renderer::UInt64Array offsets )const
	{
		assert( buffers.size() == offsets.size() );
		uint32_t binding = firstBinding;

		for ( auto i = 0u; i < buffers.size(); ++i )
		{
			auto & glBuffer = static_cast< Buffer const & >( buffers[i].get() );
			m_state.m_boundVbos[binding] = { glBuffer.getBuffer(), offsets[i], &glBuffer };
			++binding;
		}

		m_state.m_boundVao = nullptr;
	}

	void CommandBuffer::bindIndexBuffer( renderer::BufferBase const & buffer
		, uint64_t offset
		, renderer::IndexType indexType )const
	{
		auto & glBuffer = static_cast< Buffer const & >( buffer );
		m_state.m_boundIbo = BufferObjectBinding{ glBuffer.getBuffer(), offset, &glBuffer };
		m_state.m_indexType = indexType;
		m_state.m_boundVao = nullptr;
	}

	void CommandBuffer::memoryBarrier( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::BufferMemoryBarrier const & transitionBarrier )const
	{
		m_commands.emplace_back( std::make_unique< BufferMemoryBarrierCommand >( after
			, before
			, transitionBarrier ) );
	}

	void CommandBuffer::memoryBarrier( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::ImageMemoryBarrier const & transitionBarrier )const
	{
		m_commands.emplace_back( std::make_unique< ImageMemoryBarrierCommand >( after
			, before
			, transitionBarrier ) );
	}

	void CommandBuffer::bindDescriptorSets( renderer::DescriptorSetCRefArray const & descriptorSets
		, renderer::PipelineLayout const & layout
		, renderer::UInt32Array const & dynamicOffsets
		, renderer::PipelineBindPoint bindingPoint )const
	{
		for ( auto & descriptorSet : descriptorSets )
		{
			m_commands.emplace_back( std::make_unique< BindDescriptorSetCommand >( descriptorSet.get()
				, layout
				, dynamicOffsets
				, bindingPoint ) );
		}
	}

	void CommandBuffer::setViewport( renderer::Viewport const & viewport )const
	{
		m_commands.emplace_back( std::make_unique< ViewportCommand >( m_device, viewport ) );
	}

	void CommandBuffer::setScissor( renderer::Scissor const & scissor )const
	{
		m_commands.emplace_back( std::make_unique< ScissorCommand >( m_device, scissor ) );
	}

	void CommandBuffer::draw( uint32_t vtxCount
		, uint32_t instCount
		, uint32_t firstVertex
		, uint32_t firstInstance )const
	{
		if ( !m_state.m_currentPipeline->hasVertexLayout() )
		{
			bindIndexBuffer( m_device.getEmptyIndexedVaoIdx(), 0u, renderer::IndexType::eUInt32 );
			m_state.m_boundVao = &m_device.getEmptyIndexedVao();
			m_commands.emplace_back( std::make_unique< BindGeometryBuffersCommand >( *m_state.m_boundVao ) );
			m_commands.emplace_back( std::make_unique< DrawIndexedCommand >( vtxCount
				, instCount
				, 0u
				, firstVertex
				, firstInstance
				, m_state.m_currentPipeline->getInputAssemblyState().topology
				, m_state.m_indexType ) );
		}
		else
		{
			if ( !m_state.m_boundVao )
			{
				doBindVao();
			}

			m_commands.emplace_back( std::make_unique< DrawCommand >( vtxCount
				, instCount
				, firstVertex
				, firstInstance
				, m_state.m_currentPipeline->getInputAssemblyState().topology ) );
		}
	}

	void CommandBuffer::drawIndexed( uint32_t indexCount
		, uint32_t instCount
		, uint32_t firstIndex
		, uint32_t vertexOffset
		, uint32_t firstInstance )const
	{
		if ( !m_state.m_currentPipeline->hasVertexLayout() )
		{
			bindIndexBuffer( m_device.getEmptyIndexedVaoIdx(), 0u, renderer::IndexType::eUInt32 );
			m_state.m_boundVao = &m_device.getEmptyIndexedVao();
			m_commands.emplace_back( std::make_unique< BindGeometryBuffersCommand >( *m_state.m_boundVao ) );
		}
		else if ( !m_state.m_boundVao )
		{
			doBindVao();
		}

		m_commands.emplace_back( std::make_unique< DrawIndexedCommand >( indexCount
			, instCount
			, firstIndex
			, vertexOffset
			, firstInstance
			, m_state.m_currentPipeline->getInputAssemblyState().topology
			, m_state.m_indexType ) );
	}

	void CommandBuffer::drawIndirect( renderer::BufferBase const & buffer
		, uint32_t offset
		, uint32_t drawCount
		, uint32_t stride )const
	{
		if ( !m_state.m_boundVao )
		{
			doBindVao();
		}

		m_commands.emplace_back( std::make_unique< DrawIndirectCommand >( buffer
			, offset
			, drawCount
			, stride
			, m_state.m_currentPipeline->getInputAssemblyState().topology ) );
	}

	void CommandBuffer::drawIndexedIndirect( renderer::BufferBase const & buffer
		, uint32_t offset
		, uint32_t drawCount
		, uint32_t stride )const
	{
		if ( !m_state.m_currentPipeline->hasVertexLayout() )
		{
			bindIndexBuffer( m_device.getEmptyIndexedVaoIdx(), 0u, renderer::IndexType::eUInt32 );
			m_state.m_boundVao = &m_device.getEmptyIndexedVao();
			m_commands.emplace_back( std::make_unique< BindGeometryBuffersCommand >( *m_state.m_boundVao ) );
		}
		else if ( !m_state.m_boundVao )
		{
			doBindVao();
		}

		m_commands.emplace_back( std::make_unique< DrawIndexedIndirectCommand >( buffer
			, offset
			, drawCount
			, stride
			, m_state.m_currentPipeline->getInputAssemblyState().topology
			, m_state.m_indexType ) );
	}

	void CommandBuffer::copyToImage( renderer::BufferImageCopyArray const & copyInfo
		, renderer::BufferBase const & src
		, renderer::Texture const & dst )const
	{
		m_commands.emplace_back( std::make_unique< CopyBufferToImageCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::copyToBuffer( renderer::BufferImageCopyArray const & copyInfo
		, renderer::Texture const & src
		, renderer::BufferBase const & dst )const
	{
		m_commands.emplace_back( std::make_unique< CopyImageToBufferCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::copyBuffer( renderer::BufferCopy const & copyInfo
		, renderer::BufferBase const & src
		, renderer::BufferBase const & dst )const
	{
		m_commands.emplace_back( std::make_unique< CopyBufferCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::copyImage( renderer::ImageCopy const & copyInfo
		, renderer::Texture const & src
		, renderer::ImageLayout srcLayout
		, renderer::Texture const & dst
		, renderer::ImageLayout dstLayout )const
	{
		m_commands.emplace_back( std::make_unique< CopyImageCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::blitImage( renderer::Texture const & srcImage
		, renderer::ImageLayout srcLayout
		, renderer::Texture const & dstImage
		, renderer::ImageLayout dstLayout
		, std::vector< renderer::ImageBlit > const & regions
		, renderer::Filter filter )const
	{
		m_commands.emplace_back( std::make_unique< BlitImageCommand >( m_device
			, srcImage
			, dstImage
			, regions
			, filter ) );
	}

	void CommandBuffer::resetQueryPool( renderer::QueryPool const & pool
		, uint32_t firstQuery
		, uint32_t queryCount )const
	{
		m_commands.emplace_back( std::make_unique< ResetQueryPoolCommand >( pool
			, firstQuery
			, queryCount ) );
	}

	void CommandBuffer::beginQuery( renderer::QueryPool const & pool
		, uint32_t query
		, renderer::QueryControlFlags flags )const
	{
		m_commands.emplace_back( std::make_unique< BeginQueryCommand >( pool
			, query
			, flags ) );
	}

	void CommandBuffer::endQuery( renderer::QueryPool const & pool
		, uint32_t query )const
	{
		m_commands.emplace_back( std::make_unique< EndQueryCommand >( pool
			, query ) );
	}

	void CommandBuffer::writeTimestamp( renderer::PipelineStageFlag pipelineStage
		, renderer::QueryPool const & pool
		, uint32_t query )const
	{
		m_commands.emplace_back( std::make_unique< WriteTimestampCommand >( pipelineStage
			, pool
			, query ) );
	}

	void CommandBuffer::pushConstants( renderer::PipelineLayout const & layout
		, renderer::PushConstantsBufferBase const & pcb )const
	{
		if ( m_state.m_currentPipeline || m_state.m_currentComputePipeline )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( layout
				, pcb ) );
		}
		else
		{
			m_state.m_pushConstantBuffers.emplace_back( &layout, &pcb );
		}
	}

	void CommandBuffer::dispatch( uint32_t groupCountX
		, uint32_t groupCountY
		, uint32_t groupCountZ )const
	{
		m_commands.emplace_back( std::make_unique< DispatchCommand >( groupCountX
			, groupCountY 
			, groupCountZ ) );
	}

	void CommandBuffer::dispatchIndirect( renderer::BufferBase const & buffer
		, uint32_t offset )const
	{
		m_commands.emplace_back( std::make_unique< DispatchIndirectCommand >( buffer
			, offset ) );
	}

	void CommandBuffer::setLineWidth( float width )const
	{
		m_commands.emplace_back( std::make_unique< SetLineWidthCommand >( width ) );
	}

	void CommandBuffer::setDepthBias( float constantFactor
		, float clamp
		, float slopeFactor )const
	{
		m_commands.emplace_back( std::make_unique< SetDepthBiasCommand >( constantFactor
			, clamp
			, slopeFactor ) );
	}

	void CommandBuffer::initialiseGeometryBuffers()const
	{
		for ( auto & vao : m_state.m_vaos )
		{
			vao.get().initialise();
		}

		m_state.m_vaos.clear();
	}

	void CommandBuffer::doBindVao()const
	{
		m_state.m_boundVao = m_state.m_currentPipeline->findGeometryBuffers( m_state.m_boundVbos, m_state.m_boundIbo );

		if ( !m_state.m_boundVao )
		{
			m_state.m_boundVao = &m_state.m_currentPipeline->createGeometryBuffers( m_state.m_boundVbos, m_state.m_boundIbo, m_state.m_indexType ).get();
			m_state.m_vaos.emplace_back( *m_state.m_boundVao );
		}
		else if ( m_state.m_boundVao->getVao() == GL_INVALID_INDEX )
		{
			auto it = std::find_if( m_state.m_vaos.begin()
				, m_state.m_vaos.end()
				, [this]( GeometryBuffersRef const & lookup )
				{
					return &lookup.get() == m_state.m_boundVao;
				} );

			if ( it == m_state.m_vaos.end() )
			{
				m_state.m_vaos.emplace_back( *m_state.m_boundVao );
			}
		}

		m_commands.emplace_back( std::make_unique< BindGeometryBuffersCommand >( *m_state.m_boundVao ) );
	}
}
