/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_SsaoPostEffect___
#define ___C3D_SsaoPostEffect___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Miscellaneous/PostEffect.hpp>
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
		struct BloomPostEffectSurface
		{
			BloomPostEffectSurface( Castor3D::Engine & p_engine );
			bool Initialise( Castor3D::RenderTarget & p_renderTarget, Castor::Size const & p_size, uint32_t p_index, Castor3D::SamplerSPtr p_sampler );
			void Cleanup();

			Castor3D::FrameBufferSPtr m_fbo;
			Castor3D::TextureUnit m_colourTexture;
			Castor3D::TextureAttachmentSPtr m_colourAttach;
			Castor::Size m_size;
		};

		using SurfaceArray = std::array< BloomPostEffectSurface, FILTER_COUNT >;

	public:
		BloomPostEffect( Castor3D::RenderSystem & p_renderSystem, Castor3D::RenderTarget & p_renderTarget, Castor3D::Parameters const & p_param );
		virtual ~BloomPostEffect();
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
