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
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>

#ifndef _WIN32
#	define C3D_Bloom_API
#else
#	ifdef BloomPostEffect_EXPORTS
#		define C3D_Bloom_API __declspec( dllexport )
#	else
#		define C3D_Bloom_API __declspec( dllimport )
#	endif
#endif

namespace Bloom
{
	static const uint32_t FILTER_COUNT = 4;
	static const uint32_t KERNEL_SIZE = 3;

	class BloomPostEffect
		: public Castor3D::PostEffect
	{
		using SurfaceArray = std::array< PostEffectSurface, FILTER_COUNT >;

	public:
		BloomPostEffect( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & p_renderSystem, Castor3D::Parameters const & p_param );
		virtual ~BloomPostEffect();
		static Castor3D::PostEffectSPtr Create( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & p_renderSystem, Castor3D::Parameters const & p_param );
		/**
		 *\copydoc		Castor3D::PostEffect::Initialise
		 */
		virtual bool Initialise();
		/**
		 *\copydoc		Castor3D::PostEffect::Cleanup
		 */
		virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::PostEffect::Apply
		 */
		virtual bool Apply( Castor3D::FrameBuffer & p_framebuffer );

	private:
		/**
		 *\copydoc		Castor3D::PostEffect::DoWriteInto
		 */
		virtual bool DoWriteInto( Castor::TextFile & p_file );

	public:
		static Castor::String Type;
		static Castor::String Name;

	private:
		bool DoHiPassFilter( Castor3D::TextureLayout const & p_origin );
		void DoDownSample( Castor3D::TextureLayout const & p_origin );
		void DoBlur( Castor3D::TextureLayout const & p_origin, SurfaceArray & p_sources, SurfaceArray & p_destinations, Castor3D::OneFloatFrameVariableSPtr p_offset, float p_offsetValue );
		void DoCombine( Castor3D::TextureLayout const & p_origin );
		Castor3D::SamplerSPtr DoCreateSampler( bool p_linear );

		Castor3D::SamplerSPtr m_linearSampler;
		Castor3D::SamplerSPtr m_nearestSampler;

		Castor3D::ShaderProgramWPtr m_hiPassProgram;
		Castor3D::OneIntFrameVariableSPtr m_hiPassMapDiffuse;

		Castor3D::ShaderProgramWPtr m_filterProgram;
		Castor3D::OneIntFrameVariableSPtr m_filterMapDiffuse;
		Castor3D::OneFloatFrameVariableSPtr m_filterCoefficients;
		Castor3D::OneFloatFrameVariableSPtr m_filterOffsetX;
		Castor3D::OneFloatFrameVariableSPtr m_filterOffsetY;

		Castor3D::ShaderProgramWPtr m_combineProgram;

		Castor3D::Viewport m_viewport;
		Castor3D::PipelineSPtr m_pipeline;
		Castor3D::BufferDeclaration m_declaration;
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_vertices;
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		Castor3D::GeometryBuffersSPtr m_geometryBuffers;
		Castor::real m_buffer[12];
		SurfaceArray m_hiPassSurfaces;
		SurfaceArray m_blurSurfaces;
		std::array< float, KERNEL_SIZE > m_kernel;
		float m_offsetX;
		float m_offsetY;
	};
}

#endif
