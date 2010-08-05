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
#ifndef ___C3D_Module_Animation___
#define ___C3D_Module_Animation___

#include "../Module_General.h"

namespace Castor3D
{
	//! Class which represents the moving objects
	class MovingObject;
	//! The class which manages key frames
	class KeyFrame;
	//! Animation manager class
	class Animation;
	//! Class which represents the animated objects
	class AnimatedObject;
	//! Class which represents the animated object groups
	class AnimatedObjectGroup;

	//! Animation State Enum
	/*!
	The enumeration which defines all the states of an animation : playing, stopped, paused
	*/
	typedef enum AnimationState
	{
		//! Playing animation state
		AnimationPlaying,
		//! Stopped animation state
		AnimationStopped,
		//! Paused animation state
		AnimationPaused
	} AnimationState;

	//! KeyFrame pointer map, sorted by time index
	typedef std::map	<float, KeyFrame *>				KeyFrameMap;
	//! AnimatedObject pointer map, sorted by name
	typedef std::map	<String, AnimatedObject *>		AnimatedObjectStrMap;
	//! AnimatedObjectGroup pointer map, sorted by name
	typedef std::map	<String, AnimatedObjectGroup *>	AnimatedObjectGroupStrMap;
	//! MovingObject pointer map, sorted by name
	typedef std::map	<String, MovingObject *>		MovingObjectStrMap;
	//! Animation pointer map, sorted by name
	typedef std::map	<String, Animation *>			AnimationStrMap;
}

#endif

