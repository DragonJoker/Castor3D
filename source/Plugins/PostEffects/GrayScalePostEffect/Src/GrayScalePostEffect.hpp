/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GrayScalePostEffect___
#define ___C3D_GrayScalePostEffect___

#include <PostEffect/PostEffect.hpp>
#include <PostEffect/PostEffectSurface.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>

namespace GrayScale
{
	class PostEffect
		: public castor3d::PostEffect
	{
	public:
		PostEffect( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );
		~PostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & renderTarget
			, castor3d::RenderSystem & renderSystem
			, castor3d::Parameters const & params );

	private:
		/**
		*\copydoc		castor3d::PostEffect::doInitialise
		*/
		bool doInitialise( castor3d::RenderPassTimer const & timer ) override;
		/**
		*\copydoc		castor3d::PostEffect::doCleanup
		*/
		void doCleanup() override;
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & p_file ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::SamplerSPtr m_sampler;
		castor3d::PostEffectSurface m_surface;
		renderer::RenderPassPtr m_renderPass;
		std::unique_ptr< castor3d::RenderQuad > m_quad;
	};
}

#endif
