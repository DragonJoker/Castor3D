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

#include "GlBindable.hpp"

#include <Size.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		OpenGL render buffer class.
	\~french
	\brief		Tampon de rendu OpenGL.
	*/
	class GlRenderBuffer
		: public Bindable< std::function< bool( int, uint32_t * ) >,
						   std::function< bool( int, uint32_t const * ) >,
						   std::function< bool( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< bool( int, uint32_t * ) >,
									   std::function< bool( int, uint32_t const * ) >,
									   std::function< bool( uint32_t ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL api.
		 *\param[in]	p_internal		The OpenGL pixels format.
		 *\param[in]	p_renderBuffer	The parent render buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_gl			L'api OpenGL.
		 *\param[in]	p_internal		Le format OpenGL des pixels.
		 *\param[in]	p_renderBuffer	Le tampon de rendu parent.
		 */
		GlRenderBuffer( OpenGl & p_gl, eGL_RENDERBUFFER_STORAGE p_internal, Castor3D::RenderBuffer & p_renderBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~GlRenderBuffer();
		/**
		 *\~english
		 *\brief		Initialises the render buffer to given size.
		 *\param[in]	p_size	The size.
		 *\return		\p true on Ok.
		 *\~french
		 *\brief		Initialise le tampon de rendu à la taille donnée.
		 *\param[in]	p_size	Les dimensions.
		 *\return		\p true si tout s'est bien passé.
		 */
		bool Initialise( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleans the render buffer up.
		 *\~french
		 *\brief		Nettoie le tampon de rendu.
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Resizes the render buffer to given size.
		 *\param[in]	p_size	The size.
		 *\return		\p true on Ok.
		 *\~french
		 *\brief		Redimensionne le tampon de rendu à la taille donnée.
		 *\param[in]	p_size	Les dimensions.
		 *\return		\p true si tout s'est bien passé.
		 */
		bool Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\return		The OpenGL pixels format.
		 *\~french
		 *\return		Le format OpenGL des pixels.
		 */
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
