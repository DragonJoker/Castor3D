/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_Reproject_H___
#define ___C3DSMAA_Reproject_H___

#include "SmaaConfig.hpp"

#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <RenderToTexture/RenderQuad.hpp>

#include <GlslShader.hpp>

namespace smaa
{
	class Reproject
		: public castor3d::RenderQuad
	{
	public:
		Reproject( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & currentColourView
			, renderer::TextureView const & previousColourView
			, renderer::TextureView const * velocityView
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex );
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::TextureLayoutSPtr getSurface()const
		{
			return m_surface.colourTexture;
		}

	private:
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;

	private:
		renderer::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
		renderer::TextureView const & m_currentColourView;
		renderer::TextureView const & m_previousColourView;
		renderer::TextureView const * m_velocityView;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
