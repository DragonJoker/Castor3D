/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Command/CommandBuffer.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Encapsulation d'un VkCommandBuffer.
	*/
	class CommandBuffer
		: public renderer::CommandBuffer
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] pool
		*	Le pool de tampons de commandes.
		*\param[in] primary
		*	Dit si le tampon est un tampon de commandes primaire (\p true) ou secondaire (\p false).
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] pool
		*	The pool from which the command buffer is allocated.
		*\param[in] primary
		*	Specifies if the command buffer is primary (\p true) or secondary (\p false).
		*/
		CommandBuffer( Device const & device
			, CommandPool const & pool
			, bool primary );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~CommandBuffer();
		/**
		*\copydoc	renderer::CommandBuffer:begin
		*/
		bool begin( renderer::CommandBufferUsageFlags flags = 0u )const override;
		/**
		*\copydoc	renderer::CommandBuffer:begin
		*/
		bool begin( renderer::CommandBufferUsageFlags flags
			, renderer::RenderPass const & renderPass
			, uint32_t subpass
			, renderer::FrameBuffer const & frameBuffer
			, bool occlusionQueryEnable = false
			, renderer::QueryControlFlags queryFlags = 0u
			, renderer::QueryPipelineStatisticFlags pipelineStatistics = 0u )const override;
		/**
		*\copydoc	renderer::CommandBuffer:end
		*/
		bool end()const override;
		/**
		*\copydoc	renderer::CommandBuffer:reset
		*/
		bool reset( renderer::CommandBufferResetFlags flags = 0u )const override;
		/**
		*\copydoc	renderer::CommandBuffer:beginRenderPass
		*/
		void beginRenderPass( renderer::RenderPass const & renderPass
			, renderer::FrameBuffer const & frameBuffer
			, renderer::ClearValueArray const & clearValues
			, renderer::SubpassContents contents )const override;
		/**
		*\copydoc	renderer::CommandBuffer:nextSubpass
		*/
		void nextSubpass( renderer::SubpassContents contents )const override;
		/**
		*\copydoc	renderer::CommandBuffer:endRenderPass
		*/
		void endRenderPass()const override;
		/**
		*\copydoc	renderer::CommandBuffer:executeCommands
		*/
		void executeCommands( renderer::CommandBufferCRefArray const & commands )const override;
		/**
		*\copydoc	renderer::CommandBuffer:clear
		*/
		void clear( renderer::TextureView const & image
			, renderer::RgbaColour const & colour )const override;
		/**
		*\copydoc	renderer::CommandBuffer:memoryBarrier
		*/
		void memoryBarrier( renderer::PipelineStageFlags after
			, renderer::PipelineStageFlags before
			, renderer::BufferMemoryBarrier const & transitionBarrier )const override;
		/**
		*\copydoc	renderer::CommandBuffer:memoryBarrier
		*/
		void memoryBarrier( renderer::PipelineStageFlags after
			, renderer::PipelineStageFlags before
			, renderer::ImageMemoryBarrier const & transitionBarrier )const override;
		/**
		*\copydoc	renderer::CommandBuffer:bindPipeline
		*/
		void bindPipeline( renderer::Pipeline const & pipeline
			, renderer::PipelineBindPoint bindingPoint = renderer::PipelineBindPoint::eGraphics )const override;
		/**
		*\copydoc	renderer::CommandBuffer:bindGeometryBuffers
		*/
		void bindGeometryBuffers( renderer::GeometryBuffers const & geometryBuffers )const override;
		/**
		*\copydoc	renderer::CommandBuffer:bindDescriptorSet
		*/
		void bindDescriptorSet( renderer::DescriptorSet const & descriptorSet
			, renderer::PipelineLayout const & layout
			, renderer::PipelineBindPoint bindingPoint = renderer::PipelineBindPoint::eGraphics )const override;
		/**
		*\copydoc	renderer::CommandBuffer:setViewport
		*/
		void setViewport( renderer::Viewport const & viewport )const override;
		/**
		*\copydoc	renderer::CommandBuffer:setScissor
		*/
		void setScissor( renderer::Scissor const & scissor )const override;
		/**
		*\copydoc	renderer::CommandBuffer:draw
		*/
		void draw( uint32_t vtxCount
			, uint32_t instCount
			, uint32_t firstVertex
			, uint32_t firstInstance )const override;
		/**
		*\copydoc	renderer::CommandBuffer:drawIndexed
		*/
		void drawIndexed( uint32_t indexCount
			, uint32_t instCount
			, uint32_t firstIndex
			, uint32_t vertexOffset
			, uint32_t firstInstance )const override;
		/**
		*\copydoc	renderer::CommandBuffer:copyToImage
		*/
		void copyToImage( renderer::BufferImageCopy const & copyInfo
			, renderer::BufferBase const & src
			, renderer::TextureView const & dst )const override;
		/**
		*\copydoc	renderer::CommandBuffer:copyToBuffer
		*/
		void copyToBuffer( renderer::BufferImageCopy const & copyInfo
			, renderer::TextureView const & src
			, renderer::BufferBase const & dst )const override;
		/**
		*\copydoc	renderer::CommandBuffer:copyBuffer
		*/
		void copyBuffer( renderer::BufferCopy const & copyInfo
			, renderer::BufferBase const & src
			, renderer::BufferBase const & dst )const override;
		/**
		*\copydoc	renderer::CommandBuffer:copyImage
		*/
		void copyImage( renderer::ImageCopy const & copyInfo
			, renderer::TextureView const & src
			, renderer::TextureView const & dst )const override;
		/**
		*\copydoc	renderer::CommandBuffer:blitImage
		*/
		void blitImage( renderer::ImageBlit const & blit
			, renderer::TextureAttachment const & src
			, renderer::TextureAttachment const & dst
			, renderer::Filter filter )const override;
		/**
		*\copydoc	renderer::CommandBuffer:resetQueryPool
		*/
		void resetQueryPool( renderer::QueryPool const & pool
			, uint32_t firstQuery
			, uint32_t queryCount )const override;
		/**
		*\copydoc	renderer::CommandBuffer:beginQuery
		*/
		void beginQuery( renderer::QueryPool const & pool
			, uint32_t query
			, renderer::QueryControlFlags flags )const override;
		/**
		*\copydoc	renderer::CommandBuffer:endQuery
		*/
		void endQuery( renderer::QueryPool const & pool
			, uint32_t query )const override;
		/**
		*\copydoc	renderer::CommandBuffer:writeTimestamp
		*/
		void writeTimestamp( renderer::PipelineStageFlag pipelineStage
			, renderer::QueryPool const & pool
			, uint32_t query )const override;
		/**
		*\copydoc	renderer::CommandBuffer:pushConstants
		*/
		void pushConstants( renderer::PipelineLayout const & layout
			, renderer::PushConstantsBuffer const & pcb )const override;
		/**
		*\copydoc	renderer::CommandBuffer:dispatch
		*/
		void dispatch( uint32_t groupCountX
			, uint32_t groupCountY
			, uint32_t groupCountZ )const override;
		/**
		*\~french
		*\return
		*	Le pipeline actuellement actif.
		*\~english
		*\return
		*	The currently active pipeline.
		*/
		inline Pipeline const & getCurrentPipeline()const
		{
			assert( m_currentPipeline && "No pipeline bound." );
			return *m_currentPipeline;
		}
		/**
		*\~french
		*\return
		*	Dit si un pipeline est actif.
		*\~english
		*\return
		*	Tells if a pipeline is active.
		*/
		inline bool isPipelineBound()const
		{
			return m_currentPipeline != nullptr;
		}
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkCommandBuffer.
		*\~english
		*\brief
		*	VkCommandBuffer implicit cast operator.
		*/
		inline operator VkCommandBuffer const &( )const
		{
			return m_commandBuffer;
		}

	private:
		Device const & m_device;
		CommandPool const & m_pool;
		VkCommandBuffer m_commandBuffer{};
		mutable Pipeline const * m_currentPipeline{ nullptr };
		mutable VkCommandBufferInheritanceInfo m_inheritanceInfo;
	};
}
