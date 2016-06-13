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
#ifndef ___GL_DEPTH_RENDER_BUFFER_H___
#define ___GL_DEPTH_RENDER_BUFFER_H___

#include "FrameBuffer/GlRenderBuffer.hpp"

#include <FrameBuffer/DepthStencilRenderBuffer.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render buffer receiving depth and/or stencil component
	\~french
	\brief		Render tampon recevant une composante de profondeur et/ou de stencil
	*/
	class GlDepthStencilRenderBuffer
		: public Castor3D::DepthStencilRenderBuffer
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
		GlDepthStencilRenderBuffer( OpenGl & p_gl, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlDepthStencilRenderBuffer();
		/**
		 *\copydoc		Castor3D::DepthStencilRenderBuffer::Create
		 */
		virtual bool Create();
		/**
		 *\copydoc		Castor3D::DepthStencilRenderBuffer::Destroy
		 */
		virtual void Destroy();
		/**
		 *\copydoc		Castor3D::DepthStencilRenderBuffer::Initialise
		 */
		virtual bool Initialise( Castor::Size const & p_size );
		/**
		 *\copydoc		Castor3D::DepthStencilRenderBuffer::Cleanup
		 */
		virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::DepthStencilRenderBuffer::Bind
		 */
		virtual bool Bind();
		/**
		 *\copydoc		Castor3D::DepthStencilRenderBuffer::Unbind
		 */
		virtual void Unbind();
		/**
		 *\copydoc		Castor3D::DepthStencilRenderBuffer::Resize
		 */
		virtual bool Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\return		The OpenGL buffer name.
		 *\~french
		 *\return		Le nom OpenGL du tampon.
		 */
		inline uint32_t	GetGlName()const
		{
			return m_glRenderBuffer.GetGlName();
		}

	private:
		GlRenderBuffer m_glRenderBuffer;
	};
}

#endif
