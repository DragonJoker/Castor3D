/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_BlurPass_HPP___
#define ___C3D_Bloom_BlurPass_HPP___

#include <Miscellaneous/GaussianBlur.hpp>
#include <PostEffect/PostEffect.hpp>

namespace Bloom
{
	class BlurPass
	{
	public:
		BlurPass( castor3d::RenderSystem & renderSystem
			, renderer::Format format
			, castor3d::TextureLayout const & srcImage
			, castor3d::TextureLayout const & dstImage
			, renderer::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical );
		castor3d::CommandsSemaphoreArray getCommands( castor3d::RenderPassTimer const & timer
			, renderer::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const;

		inline glsl::Shader const & getVertexShader()const
		{
			return m_vertexShader;
		}

		inline glsl::Shader const & getPixelShader()const
		{
			return m_pixelShader;
		}

	public:
		static constexpr uint32_t MaxCoefficients{ 64u };

		struct Subpass
		{
			Subpass( renderer::Device const & device
				, renderer::Format format
				, renderer::TextureView const & srcView
				, renderer::TextureView const & dstView
				, renderer::RenderPass const & renderPass
				, renderer::DescriptorSetPool const & descriptorPool
				, renderer::PipelineLayout const & pipelineLayout
				, renderer::Extent2D dimensions
				, glsl::Shader const & vertexShader
				, glsl::Shader const & pixelShader
				, renderer::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
				, uint32_t index );

			renderer::FrameBufferPtr frameBuffer;
			renderer::DescriptorSetPtr descriptorSet;
			renderer::PipelinePtr pipeline;
			renderer::SamplerPtr sampler;
		};

	private:
		renderer::Device const & m_device;
		uint32_t m_blurKernelSize;
		uint32_t m_blurPassesCount;
		renderer::UniformBufferPtr< castor3d::GaussianBlur::Configuration > m_blurUbo;
		renderer::RenderPassPtr m_renderPass;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		std::vector< Subpass > m_passes;
		bool m_isVertical;
	};
}

#endif
