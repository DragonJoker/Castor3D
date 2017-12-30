/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_CUBE_TEXTURE_FACE_ATTACHMENT_H___
#define ___C3DGLES3_CUBE_TEXTURE_FACE_ATTACHMENT_H___

#include "Common/GlES3Holder.hpp"

#include <FrameBuffer/TextureAttachment.hpp>

namespace GlES3Render
{
	class GlES3CubeTextureFaceAttachment
		: public Castor3D::TextureAttachment
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\para[in]		p_gl		The OpenGL APIs.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_face		The face.
		 *\~french
		 *\brief		Constructeur
		 *\para[in]		p_gl		Les APIs OpenGL.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_face		La face.
		 */
		GlES3CubeTextureFaceAttachment( OpenGlES3 & p_gl, Castor3D::TextureLayoutSPtr p_texture, Castor3D::CubeMapFace p_face );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3CubeTextureFaceAttachment();
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
		GlES3TexDim m_glFace;
		GlES3AttachmentPoint m_glAttachmentPoint{ GlES3AttachmentPoint::eNone };
		GlES3FramebufferStatus m_glStatus{ GlES3FramebufferStatus::eIncompleteMissingAttachment };
	};
}

#endif
