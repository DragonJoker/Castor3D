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
#ifndef ___DX11_RENDER_BUFFER_ATTACHMENT_H___
#define ___DX11_RENDER_BUFFER_ATTACHMENT_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <RenderBufferAttachment.hpp>

#if defined( _WIN32 )

namespace Dx11Render
{
	class DxRenderBufferAttachment
		: public Castor3D::RenderBufferAttachment
		, public Castor::NonCopyable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_renderSystem	The RenderSystem.
		 *\param[in]	p_pRenderBuffer	The render buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_renderSystem	Le RenderSystem.
		 *\param[in]	p_pRenderBuffer	Le tampon de rendu.
		 */
		DxRenderBufferAttachment( DxRenderSystem * p_renderSystem, DxColourRenderBufferSPtr p_pRenderBuffer );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_renderSystem	The RenderSystem.
		 *\param[in]	p_pRenderBuffer	The render buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_renderSystem	Le RenderSystem.
		 *\param[in]	p_pRenderBuffer	Le tampon de rendu.
		 */
		DxRenderBufferAttachment( DxRenderSystem * p_renderSystem, DxDepthStencilRenderBufferSPtr p_pRenderBuffer );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~DxRenderBufferAttachment();
		/**
		 *\copydoc		Castor3D::TextureAttachment::Blit
		 */
		virtual bool Blit( Castor3D::FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, Castor3D::eINTERPOLATION_MODE p_interpolation );

	private:
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::DoAttach
		 */
		virtual bool DoAttach( Castor3D::FrameBufferSPtr p_pFrameBuffer );
		/**
		 *\copydoc		Castor3D::FrameBufferAttachment::DoDetach
		 */
		virtual void DoDetach();

	private:
		DxRenderBuffer & m_dxRenderBuffer;
		DxRenderSystem * m_renderSystem;
	};
}

#endif

#endif
