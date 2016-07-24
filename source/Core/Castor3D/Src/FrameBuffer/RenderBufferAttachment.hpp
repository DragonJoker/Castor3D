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
