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

#include "GlObject.hpp"

#include <Sampler.hpp>

namespace GlRender
{
	class GlSampler
		: public Castor3D::Sampler
		, private Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >;

	private:
		typedef std::function< void() > PUnbindFunction;
		typedef std::function< bool( uint32_t p_index ) > PBindFunction;

	public:
		GlSampler( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::String const & p_name );
		virtual ~GlSampler();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Bind( uint32_t p_index )const;
		virtual void Unbind( uint32_t p_index )const;

	private:
		void DoAdjustMinMipModes( eGL_INTERPOLATION_MODE & p_min, eGL_INTERPOLATION_MODE & p_mip );

	private:
		eGL_TEXDIM m_glDimension;
	};
}

#endif
