/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_CombinePass_HPP___
#define ___C3D_Bloom_CombinePass_HPP___

#include <Miscellaneous/GaussianBlur.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>

namespace Bloom
{
	class CombinePass
	{
	public:
		CombinePass( castor3d::RenderSystem & renderSystem
			, renderer::Format format
			, renderer::TextureView const & sceneView
			, renderer::TextureView const & blurView
			, renderer::Extent2D const & size
			, uint32_t blurPassesCount );
		castor3d::CommandsSemaphore getCommands( castor3d::RenderPassTimer const & timer
			, renderer::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const;

		inline glsl::Shader const & getVertexShader()const
		{
			return m_vertexShader;
		}

		inline glsl::Shader const & getPixelShader()const
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
		renderer::Device const & m_device;
		castor3d::TextureLayoutSPtr m_image;
		renderer::TextureViewPtr m_view;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::PipelinePtr m_pipeline;
		renderer::SamplerPtr m_sceneSampler;
		renderer::SamplerPtr m_blurSampler;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::CommandBufferPtr m_commandBuffer;
	};
}

#endif
