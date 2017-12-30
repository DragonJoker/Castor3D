/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_COLOUR_RENDER_BUFFER_H___
#define ___C3DGLES3_COLOUR_RENDER_BUFFER_H___

#include "FrameBuffer/GlES3RenderBuffer.hpp"

#include <FrameBuffer/ColourRenderBuffer.hpp>

namespace GlES3Render
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
	class GlES3ColourRenderBuffer
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
		GlES3ColourRenderBuffer( OpenGlES3 & p_gl, Castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3ColourRenderBuffer();
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
		virtual void Bind();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Unbind
		 */
		virtual void Unbind();
		/**
		 *\copydoc		Castor3D::ColourRenderBuffer::Resize
		 */
		virtual bool Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\return		The OpenGL buffer name.
		 *\~french
		 *\return		Le nom OpenGL du tampon.
		 */
		inline uint32_t	GetGlES3Name()const
		{
			return m_glRenderBuffer.GetGlES3Name();
		}

	private:
		GlES3RenderBuffer m_glRenderBuffer;
	};
}

#endif
