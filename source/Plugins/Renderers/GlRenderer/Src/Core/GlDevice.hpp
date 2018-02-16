/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "Core/GlContext.hpp"

#include <Core/Device.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/TessellationState.hpp>
#include <Pipeline/Viewport.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe contenant les informations liées au GPU logique.
	*/
	class Device
		: public renderer::Device
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] connection
		*	La connection à l'application.
		*/
		Device( renderer::Renderer const & renderer
			, renderer::ConnectionPtr && connection );
		/**
		*\copydoc		renderer::Device::createRenderPass
		*/
		renderer::RenderPassPtr createRenderPass( renderer::RenderPassAttachmentArray const & attaches
			, renderer::RenderSubpassPtrArray && subpasses
			, renderer::RenderPassState const & initialState
			, renderer::RenderPassState const & finalState
			, renderer::SampleCountFlag samplesCount )const override;
		/**
		*\copydoc		renderer::Device::createRenderSubpass
		*/
		renderer::RenderSubpassPtr createRenderSubpass( renderer::RenderPassAttachmentArray const & attaches
			, renderer::RenderSubpassState const & neededState )const override;
		/**
		*\copydoc		renderer::Device::createVertexLayout
		*/
		renderer::VertexLayoutPtr createVertexLayout( uint32_t bindingSlot
			, uint32_t stride
			, renderer::VertexInputRate inputRate )const override;
		/**
		*\copydoc		renderer::Device::createGeometryBuffers
		*/
		renderer::GeometryBuffersPtr createGeometryBuffers( renderer::VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > vboOffsets
			, renderer::VertexLayoutCRefArray const & layouts )const override;
		/**
		*\copydoc		renderer::Device::createGeometryBuffers
		*/
		renderer::GeometryBuffersPtr createGeometryBuffers( renderer::VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > vboOffsets
			, renderer::VertexLayoutCRefArray const & layouts
			, renderer::BufferBase const & ibo
			, uint64_t iboOffset
			, renderer::IndexType type )const override;
		/**
		*\copydoc		renderer::Device::createPipelineLayout
		*/
		renderer::PipelineLayoutPtr createPipelineLayout( renderer::DescriptorSetLayoutCRefArray const & setLayouts
			, renderer::PushConstantRangeCRefArray const & pushConstantRanges )const override;
		/**
		*\copydoc		renderer::Device::createDescriptorSetLayout
		*/
		renderer::DescriptorSetLayoutPtr createDescriptorSetLayout( renderer::DescriptorSetLayoutBindingArray && bindings )const override;
		/**
		*\copydoc		renderer::Device::createTexture
		*/
		renderer::TexturePtr createTexture( renderer::ImageLayout initialLayout )const override;
		/**
		*\copydoc		renderer::Device::createSampler
		*/
		renderer::SamplerPtr createSampler( renderer::WrapMode wrapS
			, renderer::WrapMode wrapT
			, renderer::WrapMode wrapR
			, renderer::Filter minFilter
			, renderer::Filter magFilter
			, renderer::MipmapMode mipFilter
			, float minLod
			, float maxLod
			, float lodBias
			, renderer::BorderColour borderColour
			, float maxAnisotropy
			, renderer::CompareOp compareOp )const override;
		/**
		*\copydoc		renderer::Device::createBuffer
		*/
		renderer::BufferBasePtr createBuffer( uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags memoryFlags )const override;
		/**
		*\copydoc		renderer::Device::createBufferView
		*/
		renderer::BufferViewPtr createBufferView( renderer::BufferBase const & buffer
			, renderer::PixelFormat format
			, uint32_t offset
			, uint32_t range )const override;
		/**
		*\copydoc		renderer::Device::createUniformBuffer
		*/
		renderer::UniformBufferBasePtr createUniformBuffer( uint32_t count
			, uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags memoryFlags )const override;
		/**
		*\copydoc		renderer::Device::createSwapChain
		*/
		renderer::SwapChainPtr createSwapChain( renderer::UIVec2 const & size )const override;
		/**
		*\copydoc		renderer::Device::createSemaphore
		*/
		renderer::SemaphorePtr createSemaphore()const override;
		/**
		*\copydoc	renderer::Device::createFence
		*/
		renderer::FencePtr createFence( renderer::FenceCreateFlags flags = 0 )const override;
		/**
		*\copydoc		renderer::Device::createCommandPool
		*/
		renderer::CommandPoolPtr createCommandPool( uint32_t queueFamilyIndex
			, renderer::CommandPoolCreateFlags const & flags )const override;
		/**
		*\copydoc		renderer::Device::createShaderProgram
		*/
		virtual renderer::ShaderProgramPtr createShaderProgram()const override;
		/**
		*\copydoc	renderer::Device::createQueryPool
		*/
		renderer::QueryPoolPtr createQueryPool( renderer::QueryType type
			, uint32_t count
			, renderer::QueryPipelineStatisticFlags pipelineStatistics )const override;
		/**
		*\copydoc	renderer::Device::createQueryPool
		*/
		void waitIdle()const override;
		/**
		*\copydoc	renderer::Device::perspective
		*/
		renderer::Mat4 perspective( renderer::Angle fovy
			, float aspect
			, float zNear
			, float zFar )const override;
		/**
		*\copydoc	renderer::Device::ortho
		*/
		renderer::Mat4 ortho( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const override;
		/**
		*\brief
		*	Echange les tampons.
		*/
		void swapBuffers()const;

		inline std::string const & getVendor()const
		{
			return m_context->getVendor();
		}

		inline std::string const & getRendererName()const
		{
			return m_context->getRenderer();
		}

		inline std::string const & getVersionString()const
		{
			return m_context->getVersion();
		}

		inline uint32_t getGlslVersion()const
		{
			return m_context->getGlslVersion();
		}

		inline renderer::Scissor & getCurrentScissor()const
		{
			return m_scissor;
		}

		inline renderer::Viewport & getCurrentViewport()const
		{
			return m_viewport;
		}

		inline renderer::ColourBlendState & getCurrentBlendState()const
		{
			return m_cbState;
		}

		inline renderer::DepthStencilState & getCurrentDepthStencilState()const
		{
			return m_dsState;
		}

		inline renderer::MultisampleState & getCurrentMultisampleState()const
		{
			return m_msState;
		}

		inline renderer::RasterisationState & getCurrentRasterisationState()const
		{
			return m_rsState;
		}

		inline renderer::TessellationState & getCurrentTessellationState()const
		{
			return m_tsState;
		}

		inline GLuint & getCurrentProgram()const
		{
			return m_currentProgram;
		}

	private:
		/**
		*\copydoc	renderer::Device::enable
		*/
		void doEnable()const override;
		/**
		*\copydoc	renderer::Device::disable
		*/
		void doDisable()const override;

	private:
		ContextPtr m_context;
		mutable renderer::Scissor m_scissor{ 0, 0, 0, 0 };
		mutable renderer::Viewport m_viewport{ 0, 0, 0, 0 };
		mutable renderer::ColourBlendState m_cbState;
		mutable renderer::DepthStencilState m_dsState;
		mutable renderer::MultisampleState m_msState;
		mutable renderer::RasterisationState m_rsState;
		mutable renderer::TessellationState m_tsState;
		mutable GLuint m_currentProgram;
	};
}
