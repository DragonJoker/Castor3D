/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_FRAME_BUFFER_ATTACHMENT_H___
#define ___C3D_FRAME_BUFFER_ATTACHMENT_H___

#include "Castor3DPrerequisites.hpp"
#include <Rectangle.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Description of an attachment between a frame buffer and a render buffer
	\~french
	\brief		Description d'une liaison entre un tampon d'image et un tamon de rendu
	*/
	class FrameBufferAttachment
	{
	protected:
		friend class FrameBuffer;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type	The attachment type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type	Le type d'attache
		 */
		C3D_API FrameBufferAttachment( eATTACHMENT_TYPE p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~FrameBufferAttachment();
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_attachment	The attachment point
		 *\param[in]	p_index			The attachment index
		 *\param[in]	p_frameBuffer	The frame buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image donné, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_index			L'index d'attache
		 *\param[in]	p_frameBuffer	Le tampon d'image
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_attachment, uint8_t p_index, FrameBufferSPtr p_frameBuffer );
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_attachment	The attachment point
		 *\param[in]	p_frameBuffer	The frame buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image donné, au point d'attache voulu
		 *\param[in]	p_attachment	Le point d'attache
		 *\param[in]	p_frameBuffer	Le tampon d'image
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_attachment, FrameBufferSPtr p_frameBuffer );
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		Détache le tampon de rendu du tampon d'image
		 */
		C3D_API void Detach();

	public:
		/**
		*\~english
		*\return		The data buffer.
		*\~french
		*\return		Le tampon de pixels.
		*/
		C3D_API virtual Castor::PxBufferBaseSPtr GetBuffer()const = 0;
		/**
		 *\~english
		 *\brief		Downloads the render buffer data.
		 *\return		The data buffer.
		 *\~french
		 *\brief		Récupère les données du tampon de rendu.
		 *\return		Le tampon de pixels contenant les données.
		 */
		C3D_API Castor::PxBufferBaseSPtr DownloadBuffer();
		/**
		 *\~english
		 *\brief		Blit this attachment's buffer into the given one
		 *\remark		Interpolation for depth or stencil buffer must be eINTERPOLATION_MODE_NEAREST
		 *\param[in]	p_pBuffer			The destination buffer
		 *\param[in]	p_rectSrc			The source rectangle
		 *\param[in]	p_rectDst			The destination rectangle
		 *\param[in]	p_eInterpolation	The interpolation to apply if the image is stretched
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte le tampon de cette attache dans celui donné
		 *\remark		L'interpolation pour un tampon stencil ou profondeur doit être eINTERPOLATION_MODE_NEAREST
		 *\param[in]	p_pBuffer			Le tampon destination
		 *\param[in]	p_rectSrc			Le rectangle source
		 *\param[in]	p_rectDst			Le rectangle destination
		 *\param[in]	p_eInterpolation	L'interpolation à appliquer si l'image est redimensionnée
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_eInterpolation ) = 0;
		/**
		 *\~english
		 *\return		The attachment point
		 *\~french
		 *\return		Le point d'attache
		 */
		inline eATTACHMENT_POINT GetAttachmentPoint()const
		{
			return m_point;
		}
		/**
		 *\~english
		 *\return		The attachment type.
		 *\~french
		 *\return		Le type d'attache.
		 */
		inline eATTACHMENT_TYPE GetAttachmentType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The attachment index.
		 *\~french
		 *\return		L'index d'attache.
		 */
		inline uint8_t GetAttachmentIndex()const
		{
			return m_index;
		}

	protected:
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_index			The attachment index
		 *\param[in]	p_frameBuffer	The frame buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image donné, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_frameBuffer	Le tampon d'image
		 *\param[in]	p_index			L'index d'attache
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DoAttach( FrameBufferSPtr p_frameBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		Détache le tampon de rendu du tampon d'image
		 */
		C3D_API virtual void DoDetach() = 0;
		/**
		 *\~english
		 *\return		true if there is an attached frame buffer.
		 *\~french
		 *\brief		true s'il y a un tampon d'image attaché.
		 */
		inline bool HasFrameBuffer()
		{
			return !m_frameBuffer.expired();
		}
		/**
		 *\~english
		 *\return		The attached frame buffer.
		 *\~french
		 *\brief		Le tampon d'image attaché.
		 */
		inline FrameBufferSPtr GetFrameBuffer()
		{
			return m_frameBuffer.lock();
		}

	private:
		//\~english The attachment type.	\~french Le type d'attache.
		eATTACHMENT_TYPE m_type;
		//\~english The attachment point.	\~french Le point d'attache.
		eATTACHMENT_POINT m_point;
		//\~english The attachment index.	\~french L'index d'attache.
		uint8_t m_index;
		//\~english The attached frame buffer.	\~french Le tampon d'image attaché.
		FrameBufferWPtr m_frameBuffer;
	};
}

#endif
