/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_BlendingWeightCalculation_H___
#define ___C3DSMAA_BlendingWeightCalculation_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Render/ToTexture/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>

namespace smaa
{
	class BlendingWeightCalculation
		: public castor3d::RenderQuad
	{
	public:
		BlendingWeightCalculation( castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, ashes::ImageView const & edgeDetectionView
			, castor3d::TextureLayoutSPtr depthView
			, SmaaConfig const & config );
		~BlendingWeightCalculation();
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex );
		void cpuUpdate( castor::Point4f const & subsampleIndices );
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::TextureLayoutSPtr getSurface()const
		{
			return m_surface.colourTexture;
		}

	private:
		ashes::ImageView const & m_edgeDetectionView;
		ashes::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
		ashes::SamplerPtr m_pointSampler;
		castor3d::UniformBufferOffsetT< castor::Point4f > m_ubo;
		castor3d::TextureLayoutSPtr m_areaTex;
		castor3d::TextureLayoutSPtr m_searchTex;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif
