/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_NeighbourhoodBlending_H___
#define ___C3DSMAA_NeighbourhoodBlending_H___

#include "SmaaUbo.hpp"

#include <Texture/TextureUnit.hpp>

#include <PostEffect/PostEffect.hpp>
#include <RenderToTexture/RenderQuad.hpp>

namespace smaa
{
	class NeighbourhoodBlending
		: public castor3d::RenderQuad
	{
	public:
		NeighbourhoodBlending( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & sourceView
			, renderer::TextureView const & blendView
			, renderer::TextureView const * velocityView
			, castor3d::SamplerSPtr sampler
			, uint32_t maxSubsampleIndices );

		inline renderer::RenderPass const & getRenderPass()const
		{
			return *m_renderPass;
		}

		inline renderer::FrameBuffer const & getFrameBuffer( uint32_t index )const
		{
			return *m_surfaces[index].frameBuffer;
		}

		inline castor3d::TextureLayoutSPtr getSurface( uint32_t index )const
		{
			return m_surfaces[index].colourTexture;
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
		std::vector< castor3d::PostEffectSurface > m_surfaces;
		renderer::TextureView const & m_sourceView;
		renderer::TextureView const & m_blendView;
		renderer::TextureView const * m_velocityView;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
