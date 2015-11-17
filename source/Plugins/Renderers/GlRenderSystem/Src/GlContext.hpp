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
#ifndef ___GL_CONTEXT_H___
#define ___GL_CONTEXT_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <Context.hpp>

namespace GlRender
{
	class GlContext
		:	public Castor3D::Context
		,	public Castor::NonCopyable
	{
	public:
		GlContext( GlRenderSystem & p_renderSystem, OpenGl & p_gl );
		virtual ~GlContext();

		GlContextImpl * GetImpl();
		virtual void UpdateFullScreen( bool p_bVal );
		virtual Castor::Size GetMaxSize( Castor::Size const & p_size )
		{
			return p_size;
		}

	private:
		virtual bool DoInitialise();
		virtual void DoCleanup();
		virtual void DoSetCurrent();
		virtual void DoEndCurrent();
		virtual void DoSwapBuffers();
		virtual void DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		virtual void DoSetAlphaFunc( Castor3D::eALPHA_FUNC p_eFunc, uint8_t p_byValue );
		virtual void DoCullFace( Castor3D::eFACE p_eCullFace );

	private:
		OpenGl & m_gl;
		GlContextImpl * m_pImplementation;
		GlRenderSystem * m_pGlRenderSystem;
		uint32_t m_timerQueryId[2];
		uint32_t m_queryIndex = 0;
	};
}

#endif
