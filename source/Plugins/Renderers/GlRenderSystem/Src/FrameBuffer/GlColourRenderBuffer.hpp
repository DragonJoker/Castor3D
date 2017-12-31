/*
See LICENSE file in root folder
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
