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
#ifndef ___GL_COLOUR_RENDER_BUFFER_H___
#define ___GL_COLOUR_RENDER_BUFFER_H___

#include "GlRenderBuffer.hpp"

#include <ColourRenderBuffer.hpp>

namespace GlRender
{
	class GlColourRenderBuffer
		: public Castor3D::ColourRenderBuffer
	{
	public:
		GlColourRenderBuffer( OpenGl & p_gl, Castor::ePIXEL_FORMAT p_eFormat );
		virtual ~GlColourRenderBuffer();

		virtual bool Create();
		virtual void Destroy();
		virtual bool Initialise( Castor::Size const & p_size );
		virtual void Cleanup();
		virtual bool Bind();
		virtual void Unbind();
		virtual bool Resize( Castor::Size const & p_size );

		inline uint32_t	GetGlName()const
		{
			return m_glRenderBuffer.GetGlName();
		}

	private:
		GlRenderBuffer m_glRenderBuffer;
	};
}

#endif
