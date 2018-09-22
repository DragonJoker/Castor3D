/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_NeighbourhoodBlending_H___
#define ___C3DSMAA_NeighbourhoodBlending_H___

#include "SmaaConfig.hpp"

#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <RenderToTexture/RenderQuad.hpp>

#include <GlslShader.hpp>

namespace smaa
{
	class NeighbourhoodBlending
		: public castor3d::RenderQuad
	{
	public:
		NeighbourhoodBlending( castor3d::RenderTarget & renderTarget
			, ashes::TextureView const & sourceView
			, ashes::TextureView const & blendView
			, ashes::TextureView const * velocityView
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex
			, uint32_t index );
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::TextureLayoutSPtr getSurface( uint32_t index )const
		{
			return m_surfaces[index].colourTexture;
		}

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;

	private:
		ashes::RenderPassPtr m_renderPass;
		std::vector< castor3d::PostEffectSurface > m_surfaces;
		ashes::TextureView const & m_sourceView;
		ashes::TextureView const & m_blendView;
		ashes::TextureView const * m_velocityView;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
