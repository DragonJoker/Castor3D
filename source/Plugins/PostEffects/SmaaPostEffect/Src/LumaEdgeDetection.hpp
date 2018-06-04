/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_LumaEdgeDetection_H___
#define ___C3DSMAA_LumaEdgeDetection_H___

#include "SmaaConfig.hpp"

#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <RenderToTexture/RenderQuad.hpp>

#include <GlslShader.hpp>

namespace smaa
{
	class LumaEdgeDetection
		: public castor3d::RenderQuad
	{
	public:
		LumaEdgeDetection( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & colourView
			, renderer::Texture const * predication
			, castor3d::SamplerSPtr sampler
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex );
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::TextureLayoutSPtr getSurface()const
		{
			return m_surface.colourTexture;
		}

		inline castor3d::TextureLayoutSPtr const & getDepth()const
		{
			return m_surface.depthTexture;
		}

	private:
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;

	private:
		renderer::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
		renderer::TextureView const & m_colourView;
		renderer::TextureViewPtr m_predicationView;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
