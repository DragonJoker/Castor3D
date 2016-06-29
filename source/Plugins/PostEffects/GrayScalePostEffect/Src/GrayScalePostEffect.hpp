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
#ifndef ___C3D_GrayScalePostEffect___
#define ___C3D_GrayScalePostEffect___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Miscellaneous/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>

#ifndef _WIN32
#	define C3D_GrayScale_API
#else
#	ifdef GrayScalePostEffect_EXPORTS
#		define C3D_GrayScale_API __declspec( dllexport )
#	else
#		define C3D_GrayScale_API __declspec( dllimport )
#	endif
#endif

namespace GrayScale
{
	static const uint32_t FILTER_COUNT = 4;
	static const uint32_t KERNEL_SIZE = 3;

	class GrayScalePostEffect
		: public Castor3D::PostEffect
	{
		struct PostEffectSurface
		{
			PostEffectSurface();
			bool Initialise( Castor3D::RenderTarget & p_renderTarget, Castor::Size const & p_size, uint32_t p_index, Castor3D::SamplerSPtr p_sampler );
			void Cleanup();

			Castor3D::FrameBufferSPtr m_fbo;
			Castor3D::TextureUnitSPtr m_colourTexture;
			Castor3D::TextureAttachmentSPtr m_colourAttach;
			Castor::Size m_size;
		};

	public:
		GrayScalePostEffect( Castor3D::RenderSystem & p_renderSystem, Castor3D::RenderTarget & p_renderTarget, Castor3D::Parameters const & p_param );
		~GrayScalePostEffect();
		/**
		 *\copydoc		Castor3D::PostEffect::Initialise
		 */
		bool Initialise() override;
		/**
		 *\copydoc		Castor3D::PostEffect::Cleanup
		 */
		void Cleanup() override;
		/**
		 *\copydoc		Castor3D::PostEffect::Apply
		 */
		bool Apply( Castor3D::FrameBuffer & p_framebuffer ) override;

	private:
		/**
		 *\copydoc		Castor3D::PostEffect::DoWriteInto
		 */
		bool DoWriteInto( Castor::TextFile & p_file ) override;

	public:
		static Castor::String Type;
		static Castor::String Name;

	private:
		Castor3D::ShaderProgramWPtr m_program;
		Castor3D::OneIntFrameVariableSPtr m_mapDiffuse;
		Castor3D::SamplerSPtr m_sampler;
		PostEffectSurface m_surface;
	};
}

#endif
