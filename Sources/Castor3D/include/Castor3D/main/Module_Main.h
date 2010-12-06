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
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Module_Main___
#define ___C3D_Module_Main___

#include "../Module_General.h"

namespace Castor3D
{
	class RenderWindow;		//!< The render window handler class
	class Root;				//!< The Castor3D core system
	class RendererDriver;	//!< Engine graphics driver
	class RendererServer;	//!< Manages graphics related stuff
	class Plugin;			//!< The plugin handler class
	class FrameEvent;		//!< The frame event representation
	class FrameListener;	//!< The frame event listener

	//! Projection Directions enumerator
	/*!
	The different 2 dimensions projection directions : front, back, left, right, top and bottom
	*/
	typedef enum ProjectionDirection
	{
		pdLookToFront,		//!< camera looks at the scene from the front
		pdLookToBack,		//!< camera looks at the scene from the back
		pdLookToLeft,		//!< camera looks at the scene from the left
		pdLookToRight,		//!< camera looks at the scene from the right
		pdLookToTop,		//!< camera looks at the scene from the top
		pdLookToBottom		//!< camera looks at the scene from the bottom
	} ProjectionDirection;
	
	typedef Templates::SharedPtr<RenderWindow>		RenderWindowPtr;
	typedef Templates::SharedPtr<Root>				RootPtr;
	typedef Templates::SharedPtr<RendererDriver>	RendererDriverPtr;
	typedef Templates::SharedPtr<RendererServer>	RendererServerPtr;
	typedef Templates::SharedPtr<Plugin>			PluginPtr;
	typedef Templates::SharedPtr<FrameEvent>		FrameEventPtr;
	typedef Templates::SharedPtr<FrameListener>		FrameListenerPtr;

	typedef C3DVector(			RenderWindowPtr)	RenderWindowArray;			//!< RenderWindow pointer array
	typedef C3DMap(		int,	RendererDriverPtr)	RendererDriverPtrIntMap;	//!< RendererDriver pointer map ordered by renderer type
	typedef C3DMap(		size_t, RenderWindowPtr)	RenderWindowMap;			//!< RenderWindow pointer map, sorted by index
	typedef C3DMap(		String, PluginPtr)			PluginStrMap;				//!< Plugin map, sorted by name
	typedef C3DVector(			unsigned int)		UIntArray;					//!< unsigned int array
	typedef C3DVector(			real)				FloatArray;					//!< real array
	typedef C3DVector(			FrameEventPtr)		FrameEventArray;			//!< FrameEvent pointer array
	typedef C3DVector(			FrameListenerPtr)	FrameListenerArray;			//!< FrameListener pointer array
	typedef C3DMap(		int,	String)				StrIntMap;
}

#endif

