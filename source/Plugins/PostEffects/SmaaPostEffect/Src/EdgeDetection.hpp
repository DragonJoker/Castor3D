/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_EdgeDetection_H___
#define ___C3DSMAA_EdgeDetection_H___

#include "SmaaConfig.hpp"

#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <RenderToTexture/RenderQuad.hpp>

#include <GlslShader.hpp>

namespace smaa
{
	class EdgeDetection
		: public castor3d::RenderQuad
	{
	public:
		EdgeDetection( castor3d::RenderTarget & renderTarget
			, SmaaConfig const & config );
		virtual castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex ) = 0;
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::TextureLayoutSPtr getSurface()const
		{
			return m_surface.colourTexture;
		}

		inline castor3d::TextureLayoutSPtr const & getDepth()const
		{
			return m_surface.depthTexture;
		}

	protected:
		virtual void doInitialisePipeline() = 0;

	protected:
		SmaaConfig const & m_config;
		ashes::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
	};
}

#endif
