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
#ifndef ___GL_RENDER_BUFFER_H___
#define ___GL_RENDER_BUFFER_H___

#include "GlRenderSystemPrerequisites.hpp"

#include "GlBindable.hpp"

namespace GlRender
{
	class GlRenderBuffer
		: public Castor::NonCopyable
		, public Bindable< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) >,
						   std::function< bool( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) >,
									   std::function< bool( uint32_t ) > >;

	public:
		GlRenderBuffer( OpenGl & p_gl, eGL_RENDERBUFFER_STORAGE p_internal, Castor3D::RenderBuffer & p_renderBuffer );
		~GlRenderBuffer();

		bool Initialise( Castor::Size const & p_size );
		void Cleanup();

		bool Resize( Castor::Size const & p_size );

		inline uint32_t GetInternal()const
		{
			return m_internal;
		}

	private:
		eGL_RENDERBUFFER_STORAGE m_internal;
		Castor::Size m_size;
		Castor3D::RenderBuffer & m_renderBuffer;
	};
}

#endif
