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
#ifndef ___C3D_BloomPostEffect___
#define ___C3D_BloomPostEffect___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Miscellaneous/GaussianBlur.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/MatrixUbo.hpp>

namespace Bloom
{
	static const uint32_t FILTER_COUNT = 4;

	class BloomPostEffect
		: public castor3d::PostEffect
	{
		using SurfaceArray = std::array< PostEffectSurface, FILTER_COUNT >;

	public:
		BloomPostEffect( castor3d::RenderTarget & p_renderTarget, castor3d::RenderSystem & renderSystem, castor3d::Parameters const & p_param );
		virtual ~BloomPostEffect();
		static castor3d::PostEffectSPtr create( castor3d::RenderTarget & p_renderTarget, castor3d::RenderSystem & renderSystem, castor3d::Parameters const & p_param );
		/**
		 *\copydoc		castor3d::PostEffect::Initialise
		 */
		virtual bool initialise();
		/**
		 *\copydoc		castor3d::PostEffect::Cleanup
		 */
		virtual void cleanup();
		/**
		 *\copydoc		castor3d::PostEffect::Apply
		 */
		virtual bool apply( castor3d::FrameBuffer & p_framebuffer );

	private:
		/**
		 *\copydoc		castor3d::PostEffect::doWriteInto
		 */
		virtual bool doWriteInto( castor::TextFile & p_file );

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const CombineMapPass0;
		static castor::String const CombineMapPass1;
		static castor::String const CombineMapPass2;
		static castor::String const CombineMapPass3;
		static castor::String const CombineMapScene;
		static constexpr uint32_t MaxCoefficients{ 64u };

	private:
		void doHiPassFilter( castor3d::TextureLayout const & p_origin );
		void doDownSample( castor3d::TextureLayout const & p_origin );
		void doCombine( castor3d::TextureLayout const & p_origin );
		castor3d::SamplerSPtr doCreateSampler( bool p_linear );
		bool doInitialiseHiPassProgram();
		bool doInitialiseCombineProgram();

		castor3d::SamplerSPtr m_linearSampler;
		castor3d::SamplerSPtr m_nearestSampler;

		castor3d::RenderPipelineUPtr m_hiPassPipeline;
		castor3d::PushUniform1sSPtr m_hiPassMapDiffuse;

		castor3d::GaussianBlurSPtr m_blur;

		castor3d::MatrixUbo m_matrixUbo;
		castor3d::RenderPipelineUPtr m_combinePipeline;

		castor3d::Viewport m_viewport;
		castor3d::BufferDeclaration m_declaration;
		std::array< castor3d::BufferElementGroupSPtr, 6 > m_vertices;
		castor3d::VertexBufferSPtr m_vertexBuffer;
		castor3d::GeometryBuffersSPtr m_geometryBuffers;
		castor::real m_buffer[12];
		SurfaceArray m_hiPassSurfaces;
		SurfaceArray m_blurSurfaces;
		uint32_t m_size;
	};
}

#endif
