/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_NeighbourhoodBlending_H___
#define ___C3DSMAA_NeighbourhoodBlending_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Render/ToTexture/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>

namespace smaa
{
	class NeighbourhoodBlending
		: public castor3d::RenderQuad
	{
	public:
		NeighbourhoodBlending( castor3d::RenderTarget & renderTarget
			, ashes::ImageView const & sourceView
			, ashes::ImageView const & blendView
			, ashes::ImageView const * velocityView
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
		ashes::ImageView const & m_sourceView;
		ashes::ImageView const & m_blendView;
		ashes::ImageView const * m_velocityView;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif
