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
#ifndef ___GL_TEXTURE_ATTACHMENT_H___
#define ___GL_TEXTURE_ATTACHMENT_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <TextureAttachment.hpp>

namespace GlRender
{
	class GlTextureAttachment
		:	public Castor3D::TextureAttachment
		,	public Castor::NonCopyable
	{
	public:
		GlTextureAttachment( OpenGl & p_gl, Castor3D::DynamicTextureSPtr p_pTexture );
		virtual ~GlTextureAttachment();

		virtual bool DownloadBuffer( Castor::PxBufferBaseSPtr p_pBuffer );
		virtual bool Blit( Castor3D::FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, Castor3D::eINTERPOLATION_MODE p_eInterpolation );

		inline eGL_FRAMEBUFFER_STATUS GetGlStatus()const
		{
			return m_eGlStatus;
		}
		inline eGL_TEXTURE_ATTACHMENT GetGlAttachmentPoint()const
		{
			return m_eGlAttachmentPoint;
		}

	private:
		virtual bool DoAttach( Castor3D::eATTACHMENT_POINT p_eAttachment, Castor3D::FrameBufferSPtr p_pFrameBuffer );
		virtual void DoDetach();

	private:
		eGL_TEXTURE_ATTACHMENT m_eGlAttachmentPoint;
		eGL_FRAMEBUFFER_STATUS m_eGlStatus;
		GlFrameBufferWPtr m_pGlFrameBuffer;
		OpenGl & m_gl;
	};
}

#endif
