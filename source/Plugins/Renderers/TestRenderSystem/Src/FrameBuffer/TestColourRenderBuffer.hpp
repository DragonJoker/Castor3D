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
#ifndef ___TRS_COLOUR_RENDER_BUFFER_H___
#define ___TRS_COLOUR_RENDER_BUFFER_H___

#include "TestRenderSystemPrerequisites.hpp"

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
		TestColourRenderBuffer( Castor::PixelFormat p_format );
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
