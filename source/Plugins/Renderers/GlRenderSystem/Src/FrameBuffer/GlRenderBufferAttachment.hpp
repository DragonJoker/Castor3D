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
#ifndef ___GL_RENDER_BUFFER_ATTACHMENT_H___
#define ___GL_RENDER_BUFFER_ATTACHMENT_H___

#include "Common/GlHolder.hpp"

#include <FrameBuffer/RenderBufferAttachment.hpp>

namespace GlRender
{
	class GlRenderBufferAttachment
		: public Castor3D::RenderBufferAttachment
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\para[in]		p_gl			The OpenGL APIs.
		 *\param[in]	p_renderBuffer	The render buffer.
		 *\~french
		 *\brief		Constructeur
		 *\para[in]		p_gl			Les APIs OpenGL.
		 *\param[in]	p_renderBuffer	Le tampon de rendu.
		 */
		GlRenderBufferAttachment( OpenGl & p_gl, Castor3D::RenderBufferSPtr p_renderBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlRenderBufferAttachment();
		/**
		 *\~english
		 *\return		The OpenGL buffer status.
		 *\~french
		 *\return		Le statut OpenGL du tampon.
		 */
		inline GlFramebufferStatus GetGlStatus()const
		{
			return m_glStatus;
		}
		/**
		 *\~english
		 *\return		The OpenGL attachment point.
		 *\~french
		 *\return		Le point d'attache OpenGL.
		 */
		inline GlAttachmentPoint GetGlAttachmentPoint()const
		{
			return m_glAttachmentPoint;
		}

	private:
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::Download
		 */
		void DoDownload( Castor::Position const & p_offset
			, Castor::PxBufferBase & p_buffer )const override;
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::DoAttach
		 */
		void DoAttach()override;
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::DoDetach
		 */
		void DoDetach()override;
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::DoClear
		 */
		void DoClear( Castor::Colour const & p_colour )const override;
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::DoClear
		 */
		void DoClear( float p_depth )const override;
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::DoClear
		 */
		void DoClear( int p_stencil )const override;
		/**
		*\copydoc		Castor3D::FrameBufferAttachment::DoClear
		*/
		void DoClear( float p_depth, int p_stencil )const override;

	private:
		GlAttachmentPoint m_glAttachmentPoint;
		GlFramebufferStatus m_glStatus;
	};
}

#endif
