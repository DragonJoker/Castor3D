/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "Core/GlContext.hpp"
#include "Core/GlPhysicalDevice.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Core/Device.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
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
			, PhysicalDevice const & gpu
			, renderer::ConnectionPtr && connection );
		~Device();
		/**
		*\copydoc		renderer::Device::createRenderPass
		*/
		renderer::RenderPassPtr createRenderPass( renderer::RenderPassCreateInfo createInfo )const override;
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
		*\copydoc	renderer::Device::createDescriptorPool
		*/
		renderer::DescriptorPoolPtr createDescriptorPool( renderer::DescriptorPoolCreateFlags flags
			, uint32_t maxSets
			, renderer::DescriptorPoolSizeArray poolSizes )const override;
		/**
		*\copydoc	renderer::Device::allocateMemory
		*/
		renderer::DeviceMemoryPtr allocateMemory( renderer::MemoryRequirements const & requirements
			, renderer::MemoryPropertyFlags flags )const override;
		/**
		*\copydoc		renderer::Device::createTexture
		*/
		renderer::TexturePtr createTexture( renderer::ImageCreateInfo const & createInfo )const override;
		/**
		*\copydoc	renderer::Device::getImageSubresourceLayout
		*/
		void getImageSubresourceLayout( renderer::Texture const & image
			, renderer::ImageSubresource const & subresource
			, renderer::SubresourceLayout & layout )const override;
		/**
		*\copydoc		renderer::Device::createSampler
		*/
		renderer::SamplerPtr createSampler( renderer::SamplerCreateInfo const & createInfo )const override;
		/**
		*\copydoc		renderer::Device::createBuffer
		*/
		renderer::BufferBasePtr createBuffer( uint32_t size
			, renderer::BufferTargets targets )const override;
		/**
		*\copydoc		renderer::Device::createBufferView
		*/
		renderer::BufferViewPtr createBufferView( renderer::BufferBase const & buffer
			, renderer::Format format
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
		renderer::SwapChainPtr createSwapChain( renderer::Extent2D const & size )const override;
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
		virtual renderer::ShaderModulePtr createShaderModule( renderer::ShaderStageFlag stage )const override;
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
		*\copydoc	renderer::Device::frustum
		*/
		renderer::Mat4 frustum( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const override;
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

		inline uint32_t getGlslVersion()const
		{
			return static_cast< PhysicalDevice const & >( m_gpu ).getGlslVersion();
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

		inline renderer::InputAssemblyState & getCurrentInputAssemblyState()const
		{
			return m_iaState;
		}

		inline GLuint & getCurrentProgram()const
		{
			return m_currentProgram;
		}

		inline GeometryBuffers & getEmptyIndexedVao()const
		{
			return *m_dummyIndexed.geometryBuffers;
		}

		inline renderer::BufferBase const & getEmptyIndexedVaoIdx()const
		{
			return m_dummyIndexed.indexBuffer->getBuffer();
		}

		inline GLuint getBlitSrcFbo()const
		{
			return m_blitFbos[0];
		}

		inline GLuint getBlitDstFbo()const
		{
			return m_blitFbos[1];
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
		// Mimic the behavior in Vulkan, when no IBO nor VBO is bound.
		mutable struct
		{
			renderer::BufferPtr< uint32_t > indexBuffer;
			renderer::VertexBufferPtr< renderer::Vec3 > vertexBuffer;
			GeometryBuffersPtr geometryBuffers;
		} m_dummyIndexed;
		mutable renderer::Scissor m_scissor{ 0, 0, 0, 0 };
		mutable renderer::Viewport m_viewport{ 0, 0, 0, 0 };
		mutable renderer::ColourBlendState m_cbState;
		mutable renderer::DepthStencilState m_dsState;
		mutable renderer::MultisampleState m_msState;
		mutable renderer::RasterisationState m_rsState;
		mutable renderer::TessellationState m_tsState;
		mutable renderer::InputAssemblyState m_iaState;
		mutable GLuint m_currentProgram;
		GLuint m_blitFbos[2];
	};
}
