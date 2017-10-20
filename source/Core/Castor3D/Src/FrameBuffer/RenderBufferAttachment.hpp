/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_BUFFER_ATTACHMENT_H___
#define ___C3D_RENDER_BUFFER_ATTACHMENT_H___

#include "FrameBufferAttachment.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Description of an attachment between a frame buffer and a render buffer
	\~french
	\brief		Description d'une liaison entre un tampon d'image et un tampon de rendu
	*/
	class RenderBufferAttachment
		: public FrameBufferAttachment
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderBuffer	The render buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderBuffer	Le tampon de rendu
		 */
		C3D_API explicit RenderBufferAttachment( RenderBufferSPtr p_renderBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderBufferAttachment();
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::getBuffer
		 */
		C3D_API virtual castor::PxBufferBaseSPtr getBuffer()const;
		/**
		 *\~english
		 *\brief		Retrieves the render buffer
		 *\return		The render buffer
		 *\~french
		 *\brief		Récupère le tampon de rendu
		 *\return		Le tampon de rendu
		 */
		inline RenderBufferSPtr getRenderBuffer()const
		{
			return m_pRenderBuffer.lock();
		}

	private:
		//!\~english The Attached RenderBuffer	\~french Le RenderBuffer attaché
		RenderBufferWPtr m_pRenderBuffer;
	};
}

#endif
