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
#ifndef ___C3D_Module_Main___
#define ___C3D_Module_Main___

#include "../Module_General.h"

namespace Castor3D
{
	//! The render window handler class
	class RenderWindow;
	//! The Castor3D core system
	class Root;
	//! Engine graphics driver
	class RendererDriver;
	//! Manages graphics related stuff
	class RendererServer;
	//! The plugin handler class
	class Plugin;
	//! The frame event representation
	class FrameEvent;
	//! The frame event listener
	class FrameListener;

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

	//! Frame Event Type enumeration
	/*!
	Describes the different types of frame event : pre-render, queue-render, post-render
	*/
	typedef enum FrameEventType
	{
		fetPreRender,		//!< This kind of event happens before any render
		fetQueueRender,		//!< This kind of event happens after the render queue
		fetPostRender,		//!< This kind of event happens after the buffer switch
	} FrameEventType;

	//! RenderWindow pointer array
	typedef std::vector <RenderWindow *>		RenderWindowArray;
	//! RendererDriver pointer array
	typedef std::vector <RendererDriver *>		RendererDriverPtrArray;
	//! RenderWindow pointer map, sorted by index
	typedef std::map <size_t, RenderWindow *>	RenderWindowMap;
	//! Plugin map, sorted by name
	typedef std::map <String, Plugin>			PluginStrMap;
	//! unsigned int array
	typedef std::vector <unsigned int>			UIntArray;
	//! float array
	typedef std::vector <float>					FloatArray;
	//! FrameEvent pointer array
	typedef std::vector <FrameEvent *>			FrameEventArray;
	//! FrameListener pointer array
	typedef std::vector <FrameListener *>		FrameListenerArray;
}

#endif

