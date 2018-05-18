/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_BlendingWeightCalculation_H___
#define ___C3DSMAA_BlendingWeightCalculation_H___

#include "SmaaUbo.hpp"

#include <Texture/TextureUnit.hpp>

#include <PostEffect/PostEffect.hpp>
#include <RenderToTexture/RenderQuad.hpp>

namespace smaa
{
	class BlendingWeightCalculation
		: public castor3d::RenderQuad
	{
	public:
		BlendingWeightCalculation( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & edgeDetectionView
			, castor3d::TextureLayoutSPtr depthView
			, castor3d::SamplerSPtr sampler
			, int maxSearchSteps
			, int cornerRounding
			, int maxSearchStepsDiag );
		void update( castor::Point4i const & subsampleIndices );

		inline renderer::RenderPass const & getRenderPass()const
		{
			return *m_renderPass;
		}

		inline renderer::FrameBuffer const & getFrameBuffer()const
		{
			return *m_surface.frameBuffer;
		}

		inline castor3d::TextureLayoutSPtr getSurface()const
		{
			return m_surface.colourTexture;
		}

		inline renderer::UniformBuffer< castor::Point4i > & getUbo()
		{
			return *m_ubo;
		}

		inline glsl::Shader const & getVertexShader()const
		{
			return m_vertexShader;
		}

		inline glsl::Shader const & getPixelShader()const
		{
			return m_pixelShader;
		}

	private:
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;

	private:
		renderer::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
		renderer::TextureView const & m_edgeDetectionView;
		renderer::UniformBufferPtr< castor::Point4i > m_ubo;
		castor3d::TextureLayoutSPtr m_areaTex;
		castor3d::TextureLayoutSPtr m_searchTex;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
