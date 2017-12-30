/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FilmGrainPostEffect___
#define ___C3D_FilmGrainPostEffect___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

namespace film_grain
{
	static const uint32_t FILTER_COUNT = 4;
	static const uint32_t KERNEL_SIZE = 3;

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
		bool apply( castor3d::FrameBuffer & framebuffer ) override;

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		bool doWriteInto( castor::TextFile & file ) override;
		void doGenerateNoiseTexture();

	public:
		static castor::String Type;
		static castor::String Name;

	private:
		castor3d::PushUniform1sSPtr m_mapSrc;
		castor3d::PushUniform1sSPtr m_mapNoise;
		castor3d::SamplerSPtr m_sampler2D;
		castor3d::SamplerSPtr m_sampler3D;
		castor3d::RenderPipelineSPtr m_pipeline;
		castor3d::MatrixUbo m_matrixUbo;
		PostEffectSurface m_surface;
		castor3d::TextureUnit m_noise;
		castor3d::UniformBuffer m_configUbo;
		castor3d::Uniform2f & m_pixelSize;
		castor3d::Uniform1f & m_noiseIntensity;
		castor3d::Uniform1f & m_exposure;
		castor3d::Uniform1f & m_time;
		castor::PreciseTimer m_timer;
	};
}

#endif
