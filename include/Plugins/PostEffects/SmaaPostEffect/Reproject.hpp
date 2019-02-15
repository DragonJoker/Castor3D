/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_Reproject_H___
#define ___C3DSMAA_Reproject_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/PostEffect/PostEffect.hpp>
#include <Castor3D/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/RenderToTexture/RenderQuad.hpp>

#include <ShaderWriter/Shader.hpp>

namespace smaa
{
	class Reproject
		: public castor3d::RenderQuad
	{
	public:
		Reproject( castor3d::RenderTarget & renderTarget
			, ashes::TextureView const & currentColourView
			, ashes::TextureView const & previousColourView
			, ashes::TextureView const * velocityView
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex );
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::TextureLayoutSPtr getSurface()const
		{
			return m_surface.colourTexture;
		}

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;

	private:
		ashes::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
		ashes::TextureView const & m_currentColourView;
		ashes::TextureView const & m_previousColourView;
		ashes::TextureView const * m_velocityView;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif
