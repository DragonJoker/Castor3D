/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_PREREQUISITES_SCENE_H___
#define ___C3D_PREREQUISITES_SCENE_H___

namespace Castor3D
{
	/**@name Scene */
	//@{

	class Scene;
	class SceneLoader;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;
	class Skybox;
	class AnimatedObject;
	class AnimatedObjectGroup;
	class AnimatedSkeleton;
	class AnimationInstance;
	class SkeletonAnimationInstance;
	class SkeletonAnimationInstanceObject;
	class SkeletonAnimationInstanceNode;
	class SkeletonAnimationInstanceBone;

	DECLARE_SMART_PTR( SceneNode );
	DECLARE_SMART_PTR( Scene );
	DECLARE_SMART_PTR( SceneFileContext );
	DECLARE_SMART_PTR( SceneFileParser );
	DECLARE_SMART_PTR( Skybox );
	DECLARE_SMART_PTR( AnimatedObject );
	DECLARE_SMART_PTR( AnimatedObjectGroup );
	DECLARE_SMART_PTR( AnimatedSkeleton );
	DECLARE_SMART_PTR( AnimationInstance );
	DECLARE_SMART_PTR( SkeletonAnimationInstance );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceObject );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceNode );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceBone );

	//! SceneNode pointer array
	DECLARE_VECTOR( SceneNodeSPtr, SceneNodePtr );
	//! Scene pointer map, sorted by name
	DECLARE_MAP( Castor::String, SceneSPtr, ScenePtrStr );
	//! SceneNode pointer map, sorted by name
	DECLARE_MAP( Castor::String, SceneNodeSPtr, SceneNodePtrStr );
	//! AnimatedObjectGroup pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimatedObjectGroupSPtr, AnimatedObjectGroupPtrStr );
	//! AnimatedObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! MovingObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtrStr );
	//! AnimationObject pointer array
	DECLARE_VECTOR( SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtr );
	//! Animation pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimationInstanceSPtr, AnimationInstancePtrStr );
	//! Animation pointer array
	DECLARE_VECTOR( AnimationInstanceSPtr, AnimationInstancePtr );

	//@}
}

#endif
