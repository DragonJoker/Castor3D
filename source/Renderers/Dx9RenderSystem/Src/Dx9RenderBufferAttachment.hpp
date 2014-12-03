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
#ifndef ___DX9_RENDER_BUFFER_ATTACHMENT_H___
#define ___DX9_RENDER_BUFFER_ATTACHMENT_H___

#include "Dx9RenderSystemPrerequisites.hpp"

#include <RenderBufferAttachment.hpp>

#if defined( _WIN32 )
namespace Dx9Render
{
	class DxRenderBufferAttachment
		:	public Castor3D::RenderBufferAttachment
		,	public Castor::NonCopyable
	{
	public:
		DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxColourRenderBufferSPtr p_pRenderBuffer );
		DxRenderBufferAttachment( DxRenderSystem * p_pRenderSystem, DxDepthStencilRenderBufferSPtr p_pRenderBuffer );
		virtual ~DxRenderBufferAttachment();

		virtual bool DownloadBuffer( Castor::PxBufferBaseSPtr p_pBuffer );
		virtual bool Blit( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, Castor3D::eINTERPOLATION_MODE p_eInterpolation );

		IDirect3DSurface9 * GetSurface()const;

		bool Bind();
		void Unbind();

	private:
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::FrameBufferSPtr p_pFrameBuffer );
		virtual void DoDetach();

	private:
		DWORD m_dwAttachment;
		DxFrameBufferSPtr m_pFrameBuffer;
		DxRenderBuffer	& m_dxRenderBuffer;
		IDirect3DDevice9 * m_pDevice;
		DxRenderSystem * m_pRenderSystem;
	};
}
#endif

#endif
