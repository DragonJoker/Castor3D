/* See LICENSE file in root folder */
#ifndef ___TRS_TEXTURE_ATTACHMENT_H___
#define ___TRS_TEXTURE_ATTACHMENT_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <FrameBuffer/TextureAttachment.hpp>

namespace TestRender
{
	class TestTextureAttachment
		: public castor3d::TextureAttachment
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\para[in]		p_gl		The OpenGL APIs.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Constructeur
		 *\para[in]		p_gl		Les APIs OpenGL.
		 *\param[in]	p_texture	La texture.
		 */
		TestTextureAttachment( castor3d::TextureLayoutSPtr p_texture );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~TestTextureAttachment();

	private:
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::doDownload
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
	};
}

#endif
