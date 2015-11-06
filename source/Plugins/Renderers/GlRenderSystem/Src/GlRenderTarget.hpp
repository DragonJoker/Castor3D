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
#ifndef ___GL_TARGET_RENDERER_H___
#define ___GL_TARGET_RENDERER_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "GlBuffer.hpp"
#include "GlFrameBuffer.hpp"

#include <RenderTarget.hpp>

namespace GlRender
{
	class GlRenderTarget
		: public Castor3D::RenderTarget
		, public Castor::NonCopyable
	{
	private:
		typedef bool ( GlRenderTarget:: * BeginSceneFunction )();
		typedef BeginSceneFunction PBeginSceneFunction;
		typedef void ( GlRenderTarget:: * EndSceneFunction )();
		typedef EndSceneFunction PEndSceneFunction;

	public:
		GlRenderTarget( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor3D::eTARGET_TYPE p_type );
		virtual ~GlRenderTarget();

		virtual Castor3D::RenderBufferAttachmentSPtr CreateAttachment( Castor3D::RenderBufferSPtr p_pRenderBuffer )const;
		virtual Castor3D::TextureAttachmentSPtr CreateAttachment( Castor3D::DynamicTextureSPtr p_texture )const;
		virtual Castor3D::FrameBufferSPtr CreateFrameBuffer()const;

	protected:
		bool m_bCleaned;
		OpenGl & m_gl;
		GlRenderSystem * m_renderSystem;
	};
}

#endif
