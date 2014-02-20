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
#ifndef ___Dx9_OverlayRenderer___
#define ___Dx9_OverlayRenderer___

#include "Module_Dx9Render.hpp"

namespace Dx9Render
{
	//! The Overlay renderer
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class DxOverlayRenderer : public Castor3D::OverlayRenderer
	{
	public:
		/**
		 * Constructor
		 */
		DxOverlayRenderer( DxRenderSystem * p_pRenderSystem );
		/**
		 * Destructor
		 */
		virtual ~DxOverlayRenderer();

	private:
		virtual Castor3D::ShaderProgramBaseSPtr DoGetProgram( uint32_t p_uiFlags );
		virtual void DoInitialise();
		virtual void DoCleanup();
	};
}

#endif