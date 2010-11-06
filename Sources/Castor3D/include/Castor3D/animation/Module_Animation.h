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
#ifndef ___C3D_Module_Animation___
#define ___C3D_Module_Animation___

#include "../Module_General.h"

namespace Castor3D
{
	class MovingObject;			//!< Class which represents the moving objects
	class KeyFrame;				//!< The class which manages key frames
	class Animation;			//!< Animation manager class
	class AnimatedObject;		//!< Class which represents the animated objects
	class AnimatedObjectGroup;	//!< Class which represents the animated object groups

	typedef Templates::SharedPtr<KeyFrame>				KeyFramePtr;
	typedef Templates::SharedPtr<AnimatedObject>		AnimatedObjectPtr;
	typedef Templates::SharedPtr<MovingObject>			MovingObjectPtr;
	typedef Templates::SharedPtr<Animation>				AnimationPtr;
	typedef Templates::SharedPtr<AnimatedObjectGroup>	AnimatedObjectGroupPtr;

	typedef C3DMap(	String,	AnimatedObjectGroupPtr)	AnimatedObjectGroupPtrStrMap;	//!< AnimatedObjectGroup pointer map, sorted by name
	typedef C3DMap(	String,	AnimationPtr)			AnimationPtrStrMap;				//!< Animation pointer map, sorted by name
	typedef C3DMap(	real,	KeyFramePtr)			KeyFramePtrRealMap;				//!< KeyFrame pointer map, sorted by time index
	typedef C3DMap(	String,	AnimatedObjectPtr)		AnimatedObjectPtrStrMap;		//!< AnimatedObject pointer map, sorted by name
	typedef C3DMap(	String,	MovingObjectPtr)		MovingObjectPtrStrMap;			//!< MovingObject pointer map, sorted by name
}

#endif

