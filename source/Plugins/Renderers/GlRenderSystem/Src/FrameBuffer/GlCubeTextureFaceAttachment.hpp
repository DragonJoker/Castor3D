/*
See LICENSE file in root folder
*/
#ifndef ___GL_CUBE_TEXTURE_FACE_ATTACHMENT_H___
#define ___GL_CUBE_TEXTURE_FACE_ATTACHMENT_H___

#include "Common/GlHolder.hpp"

#include <FrameBuffer/TextureAttachment.hpp>

namespace GlRender
{
	class GlCubeTextureFaceAttachment
		: public castor3d::TextureAttachment
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\para[in]		p_gl		The OpenGL APIs.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_face		The face.
		 *\param[in]	p_mipLevel	The mipmap level.
		 *\~french
		 *\brief		Constructeur
		 *\para[in]		p_gl		Les APIs OpenGL.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_face		La face.
		 *\param[in]	p_mipLevel	Le niveau de mipmap.
		 */
		GlCubeTextureFaceAttachment( OpenGl & p_gl
			, castor3d::TextureLayoutSPtr p_texture
			, castor3d::CubeMapFace p_face
			, uint32_t p_mipLevel = 0u );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlCubeTextureFaceAttachment();
		/**
		 *\~english
		 *\return		The OpenGL buffer status.
		 *\~french
		 *\return		Le statut OpenGL du tampon.
		 */
		inline GlFramebufferStatus getGlStatus()const
		{
			return m_glStatus;
		}
		/**
		 *\~english
		 *\return		The OpenGL attachment point.
		 *\~french
		 *\return		Le point d'attache OpenGL.
		 */
		inline GlAttachmentPoint getGlAttachmentPoint()const
		{
			return m_glAttachmentPoint;
		}

	private:
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::download
		 */
		void doDownload( castor::Position const & p_offset
			, castor::PxBufferBase & p_buffer )const override;
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::doAttach
		 */
		void doAttach()override;
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::doDetach
		 */
		void doDetach()override;
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::doClear
		 */
		void doClear( castor::Colour const & p_colour )const override;
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::doClear
		 */
		void doClear( float p_depth )const override;
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::doClear
		 */
		void doClear( int p_stencil )const override;
		/**
		*\copydoc		castor3d::FrameBufferAttachment::doClear
		*/
		void doClear( float p_depth, int p_stencil )const override;

	private:
		GlTexDim m_glFace;
		GlAttachmentPoint m_glAttachmentPoint{ GlAttachmentPoint::eNone };
		GlFramebufferStatus m_glStatus{ GlFramebufferStatus::eIncompleteMissingAttachment };
		uint32_t m_mipLevel;
		castor3d::CubeMapFace m_face;
	};
}

#endif
