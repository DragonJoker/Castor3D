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
#ifndef ___Gl_Context___
#define ___Gl_Context___

#include "Module_GlRender.hpp"

#include <Castor3D/Context.hpp>
#include <Castor3D/DepthStencilState.hpp>
#include <Castor3D/RasterState.hpp>
#include <Castor3D/BlendState.hpp>

namespace GlRender
{
	class GlBlendState : public Castor3D::BlendState, CuNonCopyable
	{
	private:
		OpenGl & m_gl;

	public:
		GlBlendState( OpenGl & p_gl );
		virtual ~GlBlendState();

		virtual bool Initialise(){ return true; }
		virtual void Cleanup() {}
		virtual bool Apply();
	};
	class GlRasteriserState : public Castor3D::RasteriserState, CuNonCopyable
	{
	private:
		OpenGl & m_gl;

	public:
		GlRasteriserState( OpenGl & p_gl );
		virtual ~GlRasteriserState();

		virtual bool Initialise(){ return true; }
		virtual void Cleanup() {}
		virtual bool Apply();
	};
	class GlDepthStencilState : public Castor3D::DepthStencilState, CuNonCopyable
	{
	private:
		OpenGl & m_gl;

	public:
		GlDepthStencilState( OpenGl & p_gl );
		virtual ~GlDepthStencilState();

		virtual bool Initialise(){ return true; }
		virtual void Cleanup() {}
		virtual bool Apply();
	};
	class GlContextImpl;
	class GlContext : public Castor3D::Context, CuNonCopyable
	{
	private:
		OpenGl &			m_gl;
		GlContextImpl *		m_pImplementation;
		GlRenderSystem *	m_pGlRenderSystem;

	public:
		GlContext( OpenGl & p_gl );
		virtual ~GlContext();
		
		GlContextImpl * GetImpl();
		virtual void UpdateFullScreen	( bool p_bVal );

	private:
		virtual bool DoInitialise		();
		virtual void DoCleanup			();
		virtual void DoSetCurrent		();
		virtual void DoEndCurrent		();
		virtual void DoSwapBuffers		();
		virtual void DoSetClearColour	( Castor::Colour const & p_clrClear );
		virtual void DoClear			( uint32_t p_uiTargets );
		virtual void DoBind				( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		virtual void DoSetAlphaFunc		( Castor3D::eALPHA_FUNC p_eFunc, uint8_t p_byValue );
		virtual void DoCullFace			( Castor3D::eFACE p_eCullFace );
	};
}

#endif
