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
#ifndef ___C3D_RENDER_BUFFER_ATTACHMENT_H___
#define ___C3D_RENDER_BUFFER_ATTACHMENT_H___

#include "FrameBufferAttachment.hpp"

namespace Castor3D
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
		 *\param[in]	p_pRenderBuffer	The render buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderBuffer	Le tampon de rendu
		 */
		C3D_API RenderBufferAttachment( RenderBufferSPtr p_pRenderBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderBufferAttachment();
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::GetBuffer
		 */
		C3D_API virtual Castor::PxBufferBaseSPtr GetBuffer()const;
		/**
		 *\~english
		 *\brief		Retrieves the render buffer
		 *\return		The render buffer
		 *\~french
		 *\brief		Récupère le tampon de rendu
		 *\return		Le tampon de rendu
		 */
		inline RenderBufferSPtr GetRenderBuffer()const
		{
			return m_pRenderBuffer.lock();
		}

	private:
		//!\~english The Attached RenderBuffer	\~french Le RenderBuffer attaché
		RenderBufferWPtr m_pRenderBuffer;
	};
}

#endif
