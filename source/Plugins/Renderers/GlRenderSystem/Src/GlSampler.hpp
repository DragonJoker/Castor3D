/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GL_SAMPLER_H___
#define ___GL_SAMPLER_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <Sampler.hpp>

namespace GlRender
{
	class GlSampler;

	class GlSamplerImpl
		: public Castor::OwnedBy< GlSampler >
	{
	public:
		GlSamplerImpl( GlSampler & p_sampler );
		virtual ~GlSamplerImpl();

		virtual bool Bind( eGL_TEXDIM p_dimension, uint32_t p_index ) = 0;
		virtual void Unbind() = 0;

	protected:
		void DoAdjustMinMipModes( GlSampler const & p_sampler, OpenGl const & p_gl, eGL_INTERPOLATION_MODE & p_min, eGL_INTERPOLATION_MODE & p_mip );
	};

	class GlSampler
		: public Castor3D::Sampler
		, public Holder
	{
	private:
		typedef std::function< void() > PUnbindFunction;
		typedef std::function< bool( eGL_TEXDIM p_dimension, uint32_t p_index ) > PBindFunction;

	public:
		GlSampler( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::String const & p_name );
		virtual ~GlSampler();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Bind( Castor3D::eTEXTURE_TYPE p_dimension, uint32_t p_index );
		virtual void Unbind();

	private:
		std::unique_ptr< GlSamplerImpl > m_impl;
	};
}

#endif
