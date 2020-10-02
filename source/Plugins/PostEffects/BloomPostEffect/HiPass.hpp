/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_HiPass___
#define ___C3D_Bloom_HiPass___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/Render/Passes/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>

#define Bloom_DebugHiPass 0

namespace Bloom
{
	class HiPass
		: public castor3d::RenderQuad
	{
	public:
		HiPass( castor3d::RenderSystem & renderSystem
			, castor3d::RenderDevice const & device
			, VkFormat format
			, ashes::ImageView const & sceneView
			, VkExtent2D size
			, uint32_t blurPassesCount );
		castor3d::CommandsSemaphore getCommands( castor3d::RenderPassTimer const & timer
			, uint32_t index )const;
		void accept( castor3d::PipelineVisitorBase & visitor );

		inline castor3d::ShaderModule const & getVertexShader()const
		{
			return m_vertexShader;
		}

		inline castor3d::ShaderModule const & getPixelShader()const
		{
			return m_pixelShader;
		}

		inline castor3d::TextureLayout const & getResult()const
		{
			return *m_surface.colourTexture;
		}

	private:
		ashes::ImageView const & m_sceneView;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
	};
}

#endif
