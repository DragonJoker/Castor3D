/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GL_COLOUR_RENDER_BUFFER_H___
#define ___GL_COLOUR_RENDER_BUFFER_H___

#include "FrameBuffer/GlRenderBuffer.hpp"

#include <FrameBuffer/ColourRenderBuffer.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render buffer receiving colour PixelComponents.
	\~french
	\brief		Tampon de rendu recevant une composante de couleur.
	*/
	class GlColourRenderBuffer
		: public castor3d::ColourRenderBuffer
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
		GlColourRenderBuffer( OpenGl & p_gl, castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlColourRenderBuffer();
		/**
		 *\copydoc		castor3d::ColourRenderBuffer::create
		 */
		virtual bool create();
		/**
		 *\copydoc		castor3d::ColourRenderBuffer::Destroy
		 */
		virtual void destroy();
		/**
		 *\copydoc		castor3d::ColourRenderBuffer::Initialise
		 */
		virtual bool initialise( castor::Size const & p_size );
		/**
		 *\copydoc		castor3d::ColourRenderBuffer::Cleanup
		 */
		virtual void cleanup();
		/**
		 *\copydoc		castor3d::ColourRenderBuffer::Bind
		 */
		virtual void bind();
		/**
		 *\copydoc		castor3d::ColourRenderBuffer::Unbind
		 */
		virtual void unbind();
		/**
		 *\copydoc		castor3d::ColourRenderBuffer::Resize
		 */
		virtual bool resize( castor::Size const & p_size );
		/**
		 *\~english
		 *\return		The OpenGL buffer name.
		 *\~french
		 *\return		Le nom OpenGL du tampon.
		 */
		inline uint32_t	getGlName()const
		{
			return m_glRenderBuffer.getGlName();
		}

	private:
		GlRenderBuffer m_glRenderBuffer;
	};
}

#endif
