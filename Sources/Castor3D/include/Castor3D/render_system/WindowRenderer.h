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
#ifndef ___C3D_WindowRenderer___
#define ___C3D_WindowRenderer___

#include "Renderer.h"

namespace Castor3D
{
	//! A window renderer
	/*!
	Creates the window context, renders the window, resizes the window
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API WindowRenderer : public Renderer<RenderWindow>
	{
	protected:
		friend class RenderSystem;
		/**
		 * Constructor, only RenderSystem can use it
		 */
		WindowRenderer( RenderSystem * p_rs)
			:	Renderer<RenderWindow>( p_rs)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~WindowRenderer(){ Cleanup(); }
		/**
		 * Cleans up this instance
		 */
		virtual void Cleanup(){}
		/**
		 * Initialises the context
		 */
		virtual void Initialise() = 0;
		/**
		 * Starts the render : flushes the window, ...
		 */
		virtual void StartRender() = 0;
		/**
		 * Ends the render : swaps buffers ,...
		 */
		virtual void EndRender() = 0;
		/**
		 * Resizes internally the window, id est tells the camera the new dimensions
		 */
		virtual void Resize( unsigned int p_width, unsigned int p_height) = 0;
	};
}

#endif
