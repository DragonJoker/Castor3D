/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_CombinePass_HPP___
#define ___C3D_Bloom_CombinePass_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ashespp/Image/ImageView.hpp>

namespace Bloom
{
	class CombinePass
	{
	public:
		CombinePass( castor3d::RenderDevice const & device
			, VkFormat format
			, ashes::ImageView const & sceneView
			, ashes::ImageView const & blurView
			, VkExtent2D const & size
			, uint32_t blurPassesCount );
		castor3d::CommandsSemaphore getCommands( castor3d::RenderPassTimer const & timer
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

		inline castor3d::TextureLayout const & getResult()const
		{
			return *m_image;
		}

	public:
		static castor::String const CombineMapPasses;
		static castor::String const CombineMapScene;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::TextureLayoutSPtr m_image;
		ashes::ImageView m_view;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::SamplerPtr m_sceneSampler;
		ashes::SamplerPtr m_blurSampler;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		ashes::CommandBufferPtr m_commandBuffer;
		uint32_t m_blurPassesCount;
	};
}

#endif
