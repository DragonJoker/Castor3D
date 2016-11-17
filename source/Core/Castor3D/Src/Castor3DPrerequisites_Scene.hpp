/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PREREQUISITES_SCENE_H___
#define ___C3D_PREREQUISITES_SCENE_H___

namespace Castor3D
{
	/**@name Scene */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Fog types enumeration.
	\~french
	\brief		Enumération des types de brouillard.
	*/
	enum class FogType
	{
		//!\~english	No fog.
		//!\~french		Pas de brouillard
		eDisabled,
		//!\~english	Fog intensity increases linearly with distance to camera.
		//!\~french		L'intensité du brouillard augmente linéairement avec la distance à la caméra.
		eLinear,
		//!\~english	Fog intensity increases exponentially with distance to camera.
		//!\~french		L'intensité du brouillard augmente exponentiellement avec la distance à la caméra.
		//!\~french		
		eExponential,
		//!\~english	Fog intensity increases even more with distance to camera.
		//!\~french		L'intensité du brouillard augmente encore plus avec la distance à la caméra.
		eSquaredExponential,
		CASTOR_SCOPED_ENUM_BOUNDS( eDisabled )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/11/2016
	\~english
	\brief		Billboard rendering types enumeration.
	\~french
	\brief		Enumération des types de rendu des billboards.
	*/
	enum class BillboardRenderingType
	{
		//!\~english	Billboards are rendered using instantiation.
		//!\~french		Les billboards sont dessinés en utilsant l'instantiation.
		eInstantiation,
		//!\~english	Billboards are rendered using geometry shader.
		//!\~french		Les billboards sont dessinés en utilsant les geometry shader.
		eGeometryShader,
		CASTOR_SCOPED_ENUM_BOUNDS( eInstantiation )
	};

	class Scene;
	class SceneLoader;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;
	class Skybox;
	class ParticleSystem;
	class Fog;
	class AnimatedObject;
	class AnimatedObjectGroup;
	class AnimatedSkeleton;
	class AnimatedMesh;
	class AnimationInstance;
	class SkeletonAnimationInstance;
	class SkeletonAnimationInstanceObject;
	class SkeletonAnimationInstanceNode;
	class SkeletonAnimationInstanceBone;
	class MeshAnimationInstance;
	class MeshAnimationInstanceSubmesh;
	class BillboardListBase;
	class BillboardList;

	DECLARE_SMART_PTR( SceneNode );
	DECLARE_SMART_PTR( Scene );
	DECLARE_SMART_PTR( SceneFileContext );
	DECLARE_SMART_PTR( SceneFileParser );
	DECLARE_SMART_PTR( Skybox );
	DECLARE_SMART_PTR( ParticleSystem );
	DECLARE_SMART_PTR( AnimatedObject );
	DECLARE_SMART_PTR( AnimatedObjectGroup );
	DECLARE_SMART_PTR( AnimatedSkeleton );
	DECLARE_SMART_PTR( AnimatedMesh );
	DECLARE_SMART_PTR( AnimationInstance );
	DECLARE_SMART_PTR( SkeletonAnimationInstance );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceObject );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceNode );
	DECLARE_SMART_PTR( SkeletonAnimationInstanceBone );
	DECLARE_SMART_PTR( MeshAnimationInstance );
	DECLARE_SMART_PTR( BillboardListBase );
	DECLARE_SMART_PTR( BillboardList );

	//! SceneNode pointer array.
	DECLARE_VECTOR( SceneNodeSPtr, SceneNodePtr );
	//! Scene pointer map, sorted by name.
	DECLARE_MAP( Castor::String, SceneSPtr, ScenePtrStr );
	//! SceneNode pointer map, sorted by name.
	DECLARE_MAP( Castor::String, SceneNodeSPtr, SceneNodePtrStr );
	//! AnimatedObjectGroup pointer map, sorted by name.
	DECLARE_MAP( Castor::String, AnimatedObjectGroupSPtr, AnimatedObjectGroupPtrStr );
	//! AnimatedObject pointer map, sorted by name.
	DECLARE_MAP( Castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! MovingObject pointer map, sorted by name.
	DECLARE_MAP( Castor::String, SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtrStr );
	//! SkeletonAnimationInstanceObject pointer array.
	DECLARE_VECTOR( SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtr );
	//! MeshAnimationInstanceSubmesh map, sorted by submesh ID.
	DECLARE_MAP( uint32_t, MeshAnimationInstanceSubmesh, MeshAnimationInstanceSubmesh );
	//! Animation pointer map, sorted by name.
	DECLARE_MAP( Castor::String, AnimationInstanceSPtr, AnimationInstancePtrStr );
	//! Skeleton animation instance pointer array.
	DECLARE_VECTOR( SkeletonAnimationInstanceSPtr, SkeletonAnimationInstancePtr );

	//@}
}

#endif
