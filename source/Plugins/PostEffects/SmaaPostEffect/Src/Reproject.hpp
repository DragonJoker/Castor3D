/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_Reproject_H___
#define ___C3DSMAA_Reproject_H___

#include "SmaaUbo.hpp"

#include <Texture/TextureUnit.hpp>

#include <PostEffect/PostEffect.hpp>
#include <RenderToTexture/RenderQuad.hpp>

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
			, castor3d::SamplerSPtr sampler
			, float reprojectionWeightScale );

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
		renderer::TextureView const & m_currentColourView;
		renderer::TextureView const & m_previousColourView;
		renderer::TextureView const * m_velocityView;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
