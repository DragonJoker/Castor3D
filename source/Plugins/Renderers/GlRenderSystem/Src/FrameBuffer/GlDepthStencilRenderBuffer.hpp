/*
See LICENSE file in root folder
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
	\brief		Render buffer receiving depth and/or stencil PixelComponents
	\~french
	\brief		Render tampon recevant une composante de profondeur et/ou de stencil
	*/
	class GlDepthStencilRenderBuffer
		: public castor3d::DepthStencilRenderBuffer
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
		GlDepthStencilRenderBuffer( OpenGl & p_gl, castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlDepthStencilRenderBuffer();
		/**
		 *\copydoc		castor3d::DepthStencilRenderBuffer::create
		 */
		virtual bool create();
		/**
		 *\copydoc		castor3d::DepthStencilRenderBuffer::Destroy
		 */
		virtual void destroy();
		/**
		 *\copydoc		castor3d::DepthStencilRenderBuffer::Initialise
		 */
		virtual bool initialise( castor::Size const & p_size );
		/**
		 *\copydoc		castor3d::DepthStencilRenderBuffer::Cleanup
		 */
		virtual void cleanup();
		/**
		 *\copydoc		castor3d::DepthStencilRenderBuffer::Bind
		 */
		virtual void bind();
		/**
		 *\copydoc		castor3d::DepthStencilRenderBuffer::Unbind
		 */
		virtual void unbind();
		/**
		 *\copydoc		castor3d::DepthStencilRenderBuffer::Resize
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
