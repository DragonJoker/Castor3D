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
#ifndef ___TRS_COLOUR_RENDER_BUFFER_H___
#define ___TRS_COLOUR_RENDER_BUFFER_H___

#include <FrameBuffer/ColourRenderBuffer.hpp>

namespace TestRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render buffer receiving colour component.
	\~french
	\brief		Tampon de rendu recevant une composante de couleur.
	*/
	class TestColourRenderBuffer
		: public Castor3D::ColourRenderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_gl		The OpenGL api.
		 *\param[in]	p_format	The pixels format.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_gl		L'api OpenGL.
		 *\param[in]	p_format	Le format des pixels.
		 */
		TestColourRenderBuffer( Castor::ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~TestColourRenderBuffer();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Create
		 */
		virtual bool Create();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Destroy
		 */
		virtual void Destroy();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Initialise
		 */
		virtual bool Initialise( Castor::Size const & p_size );
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Cleanup
		 */
		virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Bind
		 */
		virtual bool Bind();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Unbind
		 */
		virtual void Unbind();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Resize
		 */
		virtual bool Resize( Castor::Size const & p_size );
	};
}

#endif
