/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_RENDER_BUFFER_ATTACHMENT_H___
#define ___C3DGLES3_RENDER_BUFFER_ATTACHMENT_H___

#include "Common/GlES3Holder.hpp"

#include <FrameBuffer/RenderBufferAttachment.hpp>

namespace GlES3Render
{
	class GlES3RenderBufferAttachment
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
		GlES3RenderBufferAttachment( OpenGlES3 & p_gl, Castor3D::RenderBufferSPtr p_renderBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3RenderBufferAttachment();
		/**
		 *\copydoc		Castor3D::TextureAttachment::Blit
		 */
		void Blit( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, Castor3D::InterpolationMode p_interpolation )override;
		/**
		 *\~english
		 *\return		The OpenGL buffer status.
		 *\~french
		 *\return		Le statut OpenGL du tampon.
		 */
		inline GlES3FramebufferStatus GetGlES3Status()const
		{
			return m_glStatus;
		}
		/**
		 *\~english
		 *\return		The OpenGL attachment point.
		 *\~french
		 *\return		Le point d'attache OpenGL.
		 */
		inline GlES3AttachmentPoint GetGlES3AttachmentPoint()const
		{
			return m_glAttachmentPoint;
		}

	private:
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
		void DoClear( Castor3D::BufferComponent p_component )const override;

	private:
		GlES3AttachmentPoint m_glAttachmentPoint;
		GlES3FramebufferStatus m_glStatus;
	};
}

#endif
