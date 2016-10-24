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
#ifndef ___GL_RENDER_BUFFER_H___
#define ___GL_RENDER_BUFFER_H___

#include "Common/GlBindable.hpp"

#include <Graphics/Size.hpp>

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
		: public Bindable< std::function< bool( int, uint32_t * ) >
			, std::function< bool( int, uint32_t const * ) >
			, std::function< bool( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< bool( int, uint32_t * ) >
			, std::function< bool( int, uint32_t const * ) >
			, std::function< bool( uint32_t ) > >;

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
		 *\brief		Initialise le tampon de rendu � la taille donn�e.
		 *\param[in]	p_size	Les dimensions.
		 *\return		\p true si tout s'est bien pass�.
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
		 *\brief		Redimensionne le tampon de rendu � la taille donn�e.
		 *\param[in]	p_size	Les dimensions.
		 *\return		\p true si tout s'est bien pass�.
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
