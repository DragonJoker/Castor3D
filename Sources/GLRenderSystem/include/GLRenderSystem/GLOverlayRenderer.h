/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GLOverlayRenderer___
#define ___GLOverlayRenderer___

#include "Module_GLRender.h"
#include "GLRenderSystem.h"

namespace Castor3D
{
	//! The Overlay renderer
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_GL_API GLOverlayRenderer : public OverlayRenderer
	{
	protected:
		friend class GLRenderSystem;
		/**
		 * Constructor
		 */
		GLOverlayRenderer( GLRenderSystem * p_rs)
			:	OverlayRenderer( p_rs)
		{}
	public:
		/**
		 * Destructor
		 */
		virtual ~GLOverlayRenderer(){}

		virtual void DrawPanel();
		virtual void DrawBorderPanel();
		virtual void DrawText();
	};
}

#endif