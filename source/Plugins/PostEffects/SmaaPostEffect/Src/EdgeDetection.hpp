/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_EdgeDetection_H___
#define ___C3DSMAA_EdgeDetection_H___

#include "SmaaUbo.hpp"

#include <Texture/TextureUnit.hpp>

#include <PostEffect/PostEffect.hpp>
#include <RenderToTexture/RenderQuad.hpp>

namespace smaa
{
	class EdgeDetection
		: public castor3d::RenderQuad
	{
	public:
		EdgeDetection( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & colourView
			, renderer::TextureView const * predicationView
			, castor3d::SamplerSPtr sampler
			, float threshold
			, float predicationThreshold
			, float predicationScale
			, float predicationStrength );

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
		renderer::TextureView const * m_predicationView;
	};
}

#endif
