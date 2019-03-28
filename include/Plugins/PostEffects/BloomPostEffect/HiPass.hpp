/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_HiPass___
#define ___C3D_Bloom_HiPass___

#include <Castor3D/PostEffect/PostEffect.hpp>
#include <Castor3D/PostEffect/PostEffectSurface.hpp>
#include <Castor3D/RenderToTexture/RenderQuad.hpp>

#include <ShaderWriter/Shader.hpp>

#define Bloom_DebugHiPass 0

namespace Bloom
{
	class HiPass
		: public castor3d::RenderQuad
	{
	public:
		HiPass( castor3d::RenderSystem & renderSystem
			, ashes::Format format
			, ashes::TextureView const & sceneView
			, ashes::Extent2D size
			, uint32_t blurPassesCount );
		castor3d::CommandsSemaphore getCommands( castor3d::RenderPassTimer const & timer )const;

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
		ashes::Device const & m_device;
		ashes::TextureView const & m_sceneView;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		castor3d::PostEffectSurface m_surface;
	};
}

#endif
