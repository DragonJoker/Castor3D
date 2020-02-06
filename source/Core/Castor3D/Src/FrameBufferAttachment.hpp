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
	class C3D_API FrameBufferAttachment
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		FrameBufferAttachment();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~FrameBufferAttachment();
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_pFrameBuffer	The frame buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image donn�, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_pFrameBuffer	Le tampon d'image
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Attach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer );
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		D�tache le tampon de rendu du tampon d'image
		 */
		virtual void Detach();
		/**
		 *\~english
		 *\brief		Downloads the render buffer data
		 *\param[out]	p_pBuffer	Receives the buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		R�cup�re les donn�es du tampon
		 *\param[out]	p_pBuffer	Re�oit les donn�es
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool DownloadBuffer( Castor::PxBufferBaseSPtr p_pBuffer ) = 0;
		/**
		 *\~english
		 *\return		\p true if the attachment has been made
		 *\~french
		 *\return		\p true si la liaison a �t� faite
		 */
		inline bool HasFrameBuffer()const
		{
			return !m_pFrameBuffer.expired();
		}
		/**
		 *\~english
		 *\brief		Retrieves the frame buffer
		 *\return		The frame buffer
		 *\~french
		 *\brief		R�cup�re le tampon d'image
		 *\return		Le tampon d'image
		 */
		inline FrameBufferSPtr GetFrameBuffer()const
		{
			return m_pFrameBuffer.lock();
		}
		/**
		 *\~english
		 *\return		The attachment point
		 *\~french
		 *\return		Le point d'attache
		 */
		inline eATTACHMENT_POINT GetAttachmentPoint()const
		{
			return m_eAttachmentPoint;
		}
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
		 *\brief		Blitte le tampon de cette attache dans celui donn�
		 *\remark		L'interpolation pour un tampon stencil ou profondeur doit �tre eINTERPOLATION_MODE_NEAREST
		 *\param[in]	p_pBuffer			Le tampon destination
		 *\param[in]	p_rectSrc			Le rectangle source
		 *\param[in]	p_rectDst			Le rectangle destination
		 *\param[in]	p_eInterpolation	L'interpolation � appliquer si l'image est redimensionn�e
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Blit( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_eInterpolation ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_pFrameBuffer	The frame buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image donn�, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_pFrameBuffer	Le tampon d'image
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool DoAttach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		D�tache le tampon de rendu du tampon d'image
		 */
		virtual void DoDetach() = 0;

	private:
		//\~english The attachment point	\~french Le point d'attache
		eATTACHMENT_POINT m_eAttachmentPoint;
		//\~english The parent FrameBuffer	\~french Le FrameBuffer parent
		FrameBufferWPtr m_pFrameBuffer;
	};
}

#endif
