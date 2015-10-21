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
#ifndef ___DX11_TARGET_RENDERER_H___
#define ___DX11_TARGET_RENDERER_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <RenderTarget.hpp>

namespace Dx11Render
{
	class DxRenderTarget
		:	public Castor3D::RenderTarget
	{
	public:
		DxRenderTarget( DxRenderSystem * p_renderSystem, Castor3D::eTARGET_TYPE p_type );
		virtual ~DxRenderTarget();

		virtual Castor3D::RenderBufferAttachmentSPtr CreateAttachment( Castor3D::RenderBufferSPtr p_pRenderBuffer )const;
		virtual Castor3D::TextureAttachmentSPtr CreateAttachment( Castor3D::DynamicTextureSPtr p_pTexture )const;
		virtual Castor3D::FrameBufferSPtr CreateFrameBuffer()const;

	private:
		bool m_bInitialised;
		DxRenderSystem * m_renderSystem;
	};
}

#endif