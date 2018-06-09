/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_HiPass___
#define ___C3D_Bloom_HiPass___

#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <RenderToTexture/RenderQuad.hpp>

#include <GlslShader.hpp>

#define Bloom_DebugHiPass 0

namespace Bloom
{
	class HiPass
		: public castor3d::RenderQuad
	{
	public:
		HiPass( castor3d::RenderSystem & renderSystem
			, renderer::Format format
			, renderer::TextureView const & sceneView
			, renderer::Extent2D size
			, uint32_t blurPassesCount );
		castor3d::CommandsSemaphore getCommands( castor3d::RenderPassTimer const & timer )const;

		inline glsl::Shader const & getVertexShader()const
		{
			return m_vertexShader;
		}

		inline glsl::Shader const & getPixelShader()const
		{
			return m_pixelShader;
		}

		inline castor3d::TextureLayout const & getResult()const
		{
			return *m_surface.colourTexture;
		}

	private:
		renderer::Device const & m_device;
		renderer::TextureView const & m_sceneView;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
		renderer::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
	};
}

#endif
