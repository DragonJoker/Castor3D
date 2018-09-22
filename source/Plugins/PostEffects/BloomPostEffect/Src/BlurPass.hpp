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
			, ashes::Format format
			, castor3d::TextureLayout const & srcImage
			, castor3d::TextureLayout const & dstImage
			, ashes::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical );
		castor3d::CommandsSemaphoreArray getCommands( castor3d::RenderPassTimer const & timer
			, ashes::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const;

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
			Subpass( ashes::Device const & device
				, ashes::Format format
				, ashes::TextureView const & srcView
				, ashes::TextureView const & dstView
				, ashes::RenderPass const & renderPass
				, ashes::DescriptorSetPool const & descriptorPool
				, ashes::PipelineLayout const & pipelineLayout
				, ashes::Extent2D dimensions
				, glsl::Shader const & vertexShader
				, glsl::Shader const & pixelShader
				, ashes::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
				, uint32_t index );

			ashes::FrameBufferPtr frameBuffer;
			ashes::DescriptorSetPtr descriptorSet;
			ashes::PipelinePtr pipeline;
			ashes::SamplerPtr sampler;
		};

	private:
		ashes::Device const & m_device;
		uint32_t m_blurKernelSize;
		uint32_t m_blurPassesCount;
		ashes::UniformBufferPtr< castor3d::GaussianBlur::Configuration > m_blurUbo;
		ashes::RenderPassPtr m_renderPass;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		std::vector< Subpass > m_passes;
		bool m_isVertical;
	};
}

#endif
