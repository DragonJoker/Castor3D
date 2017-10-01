/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
