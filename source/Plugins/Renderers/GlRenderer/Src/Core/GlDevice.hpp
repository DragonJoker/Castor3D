/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "Core/GlContext.hpp"

#include <Core/Device.hpp>

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
		*\copydoc	renderer::Device::enable
		*/
		void enable()const override;
		/**
		*\copydoc	renderer::Device::disable
		*/
		void disable()const override;
		/**
		*\copydoc		renderer::Device::createRenderPass
		*/
		renderer::RenderPassPtr createRenderPass( std::vector< renderer::PixelFormat > const & formats
			, renderer::RenderSubpassPtrArray const & subpasses
			, renderer::RenderPassState const & initialState
			, renderer::RenderPassState const & finalState
			, bool clear
			, renderer::SampleCountFlag samplesCount )const override;
		/**
		*\copydoc		renderer::Device::createRenderSubpass
		*/
		renderer::RenderSubpassPtr createRenderSubpass( std::vector< renderer::PixelFormat > const & formats
			, renderer::RenderSubpassState const & neededState )const override;
		/**
		*\copydoc		renderer::Device::createVertexLayout
		*/
		renderer::VertexLayoutPtr createVertexLayout( uint32_t bindingSlot
			, uint32_t stride )const override;
		/**
		*\copydoc		renderer::Device::createGeometryBuffers
		*/
		renderer::GeometryBuffersPtr createGeometryBuffers( renderer::VertexBufferBase const & vbo
			, uint64_t vboOffset
			, renderer::VertexLayout const & layout )const override;
		/**
		*\copydoc		renderer::Device::createGeometryBuffers
		*/
		renderer::GeometryBuffersPtr createGeometryBuffers( renderer::VertexBufferBase const & vbo
			, uint64_t vboOffset
			, renderer::VertexLayout const & layout
			, renderer::BufferBase const & ibo
			, uint64_t iboOffset
			, renderer::IndexType type )const override;
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
		renderer::PipelineLayoutPtr createPipelineLayout()const override;
		/**
		*\copydoc		renderer::Device::createPipelineLayout
		*/
		renderer::PipelineLayoutPtr createPipelineLayout( renderer::DescriptorSetLayout const & layout )const override;
		/**
		*\copydoc		renderer::Device::createPipeline
		*/
		renderer::PipelinePtr createPipeline( renderer::PipelineLayout const & layout
			, renderer::ShaderProgram const & program
			, renderer::VertexLayoutCRefArray const & vertexLayouts
			, renderer::RenderPass const & renderPass
			, renderer::PrimitiveTopology topology
			, renderer::RasterisationState const & rasterisationState
			, renderer::ColourBlendState const & colourBlendState )const override;
		/**
		*\copydoc		renderer::Device::createDescriptorSetLayout
		*/
		renderer::DescriptorSetLayoutPtr createDescriptorSetLayout( renderer::DescriptorSetLayoutBindingArray && bindings )const override;
		/**
		*\copydoc		renderer::Device::createTexture
		*/
		renderer::TexturePtr createTexture( renderer::ImageLayout initialLayout )const override;
		/**
		*\copydoc	renderer::Device::createRenderBuffer
		*/
		renderer::RenderBufferPtr createRenderBuffer( renderer::PixelFormat format
			, renderer::UIVec2 const & size )override;
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
			, float zFar )override;
		/**
		*\copydoc	renderer::Device::ortho
		*/
		renderer::Mat4 ortho( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )override;
		/**
		*\brief
		*	Echange les tampons.
		*/
		void swapBuffers()const;

		inline std::string const & getVendor()const
		{
			return m_context->getVendor();
		}

		inline std::string const & getRenderer()const
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

	private:
		ContextPtr m_context;
	};
}
