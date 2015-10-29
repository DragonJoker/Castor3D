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
#ifndef ___GL_BUFFER_BASE_H___
#define ___GL_BUFFER_BASE_H___

#include "GlRenderSystemPrerequisites.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		OpenGL buffer objects base class
	\~french
	\brief		Classe de base des buffers objects OpenGL
	*/
	template< typename T >
	class GlBufferBase
		: public Castor::NonCopyable
	{
	public:
		GlBufferBase( OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget );
		virtual ~GlBufferBase();

		bool Create();
		void Destroy();
		bool Initialise( T const * p_buffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		void Cleanup();
		bool Bind();
		void Unbind();
		bool Fill( T const * p_buffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		T  * Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		T  * Lock( eGL_LOCK p_access );
		bool Unlock();

		inline uint32_t GetGlIndex()const
		{
			return m_uiGlIndex;
		}

	private:
		uint32_t m_uiGlIndex;
		eGL_BUFFER_TARGET m_eTarget;

	protected:
		OpenGl & m_gl;
	};
}

#include "GlBufferBase.inl"

#endif
