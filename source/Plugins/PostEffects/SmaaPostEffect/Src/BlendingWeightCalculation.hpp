/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_BlendingWeightCalculation_H___
#define ___C3DSMAA_BlendingWeightCalculation_H___

#include "SmaaConfig.hpp"

#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <RenderToTexture/RenderQuad.hpp>

#include <GlslShader.hpp>

namespace smaa
{
	class BlendingWeightCalculation
		: public castor3d::RenderQuad
	{
	public:
		BlendingWeightCalculation( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & edgeDetectionView
			, castor3d::TextureLayoutSPtr depthView
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex );
		void update( castor::Point4i const & subsampleIndices );
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::TextureLayoutSPtr getSurface()const
		{
			return m_surface.colourTexture;
		}

	private:
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;

	private:
		renderer::TextureView const & m_edgeDetectionView;
		renderer::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
		renderer::SamplerPtr m_pointSampler;
		renderer::UniformBufferPtr< castor::Point4i > m_ubo;
		castor3d::TextureLayoutSPtr m_areaTex;
		castor3d::TextureLayoutSPtr m_searchTex;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
