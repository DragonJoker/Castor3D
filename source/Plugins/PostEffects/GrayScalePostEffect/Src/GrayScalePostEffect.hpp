/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GrayScalePostEffect___
#define ___C3D_GrayScalePostEffect___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>

namespace GrayScale
{
	static const uint32_t FILTER_COUNT = 4;
	static const uint32_t KERNEL_SIZE = 3;

	class GrayScalePostEffect
		: public castor3d::PostEffect
	{
	public:
		GrayScalePostEffect( castor3d::RenderTarget & p_renderTarget, castor3d::RenderSystem & renderSystem, castor3d::Parameters const & p_param );
		~GrayScalePostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & p_renderTarget, castor3d::RenderSystem & renderSystem, castor3d::Parameters const & p_param );
		/**
		 *\copydoc		castor3d::PostEffect::Initialise
		 */
		bool initialise() override;
		/**
		 *\copydoc		castor3d::PostEffect::Cleanup
		 */
		void cleanup() override;
		/**
		 *\copydoc		castor3d::PostEffect::Apply
		 */
		bool apply( castor3d::FrameBuffer & p_framebuffer ) override;

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & p_file ) override;

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::SamplerSPtr m_sampler;
		castor3d::RenderPipelineSPtr m_pipeline;
		castor3d::MatrixUbo m_matrixUbo;
		PostEffectSurface m_surface;
	};
}

#endif
