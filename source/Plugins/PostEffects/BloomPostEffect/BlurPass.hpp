/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_BlurPass_HPP___
#define ___C3D_Bloom_BlurPass_HPP___

#include <Castor3D/Render/Passes/GaussianBlur.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace Bloom
{
	using UboOffset = castor3d::UniformBufferOffsetT< castor3d::GaussianBlur::Configuration >;
	using UboOffsetArray = std::vector< UboOffset >;

	class BlurPass
	{
	public:
		BlurPass( castor3d::RenderDevice const & device
			, VkFormat format
			, castor3d::TextureLayout const & srcImage
			, castor3d::TextureLayout const & dstImage
			, VkExtent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical );
		castor3d::CommandsSemaphoreArray getCommands( castor3d::RenderPassTimer const & timer
			, uint32_t & index
			, ashes::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const;
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::ShaderModule const & getVertexShader()const
		{
			return m_vertexShader;
		}

		inline castor3d::ShaderModule const & getPixelShader()const
		{
			return m_pixelShader;
		}

	public:
		static constexpr uint32_t MaxCoefficients{ 64u };

		struct Subpass
		{
			Subpass( castor3d::RenderDevice const & device
				, VkFormat format
				, ashes::ImageView const & srcView
				, ashes::ImageView const & dstView
				, ashes::RenderPass const & renderPass
				, ashes::DescriptorSetPool const & descriptorPool
				, ashes::PipelineLayout const & pipelineLayout
				, VkExtent2D dimensions
				, castor3d::ShaderModule const & vertexShader
				, castor3d::ShaderModule const & pixelShader
				, castor3d::UniformBufferOffsetT< castor3d::GaussianBlur::Configuration > const & blurUbo
				, uint32_t index );

			ashes::FrameBufferPtr frameBuffer;
			ashes::DescriptorSetPtr descriptorSet;
			ashes::GraphicsPipelinePtr pipeline;
			ashes::SamplerPtr sampler;
		};

	private:
		castor3d::RenderDevice const & m_device;
		uint32_t m_blurKernelSize;
		uint32_t m_blurPassesCount;
		UboOffsetArray m_blurUbo;
		ashes::RenderPassPtr m_renderPass;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		std::vector< Subpass > m_passes;
		bool m_isVertical;
	};
}

#endif
