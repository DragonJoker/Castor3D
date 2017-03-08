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
	\date		08/03/2017
	\~english
	\brief		Scene flags enumeration.
	\~french
	\brief		Enumération des indicateurs de scène.
	*/
	enum class SceneFlag
		: uint16_t
	{
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x0000,
		//!\~english	Linear fog.
		//!\~french		Brouillard linéaire.
		eFogLinear = 0x0001,
		//!\~english	Exponential fog.
		//!\~french		Brouillard exponentiel.
		eFogExponential = 0x0002,
		//!\~english	Squared exponential fog.
		//!\~french		Brouillard exponentiel au carré.
		eFogSquaredExponential = 0x0003,
		//!\~english	No filtering.
		//!\~french		Pas de filtrage.
		eShadowFilterRaw = 0x0004,
		//!\~english	Poisson filtering.
		//!\~french		Filtrage poisson.
		eShadowFilterPoisson = 0x0008,
		//!\~english	Stratified poisson filtering.
		//!\~french		Filtrage poisson stratifié.
		eShadowFilterStratifiedPoisson = 0x000C,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
	};
	IMPLEMENT_FLAGS( SceneFlag )
	/**
	 *\~english
	 *\brief		Gives the shadow filter type matching the given flags.
	 *\param[in]	p_flags	The scene flags.
	 *\return		The shadow filter type.
	 *\~french
	 *\brief		Récupère le type de filtrage d'ombres correspondant aux indicateurs donnés.
	 *\param[in]	p_flags	Les indicateurs de scène.
	 *\return		Le type de filtrage d'ombres.
	 */
	C3D_API GLSL::ShadowType GetShadowType( SceneFlags const & p_flags );
	/**
	 *\~english
	 *\brief		Gives the fog type matching the given flags.
	 *\param[in]	p_flags	The scene flags.
	 *\return		The fog type.
	 *\~french
	 *\brief		Récupère le type de brouillard correspondant aux indicateurs donnés.
	 *\param[in]	p_flags	Les indicateurs de scène.
	 *\return		Le type de brouillard.
	 */
	C3D_API GLSL::FogType GetFogType( SceneFlags const & p_flags );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/11/2016
	\~english
	\brief		Billboard rendering types enumeration.
	\~french
	\brief		Enumération des types de rendu des billboards.
	*/
	enum class BillboardType
	{
		//!\~english	Billboards always face the camera.
		//!\~french		Les billboards font toujours face à la caméra.
		eSpherical,
		//!\~english	Billboards rotate only on Y axis.
		//!\~french		Les billboards tournent uniquement sur l'axe Y.
		eCylindrical,
		CASTOR_SCOPED_ENUM_BOUNDS( eSpherical )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/11/2016
	\~english
	\brief		Billboard sizing types enumeration.
	\~french
	\brief		Enumération des types de dimensions des billboards.
	*/
	enum class BillboardSize
	{
		//!\~english	Billboards size changes with camera position.
		//!\~french		Les dimensions des billboards changent en fonction de la position de la caméra.
		eDynamic,
		//!\~english	Billboards size doesn't change.
		//!\~french		Les dimensions des billboards ne changent pas.
		eFixed,
		CASTOR_SCOPED_ENUM_BOUNDS( eDynamic )
	};

	class Scene;
	class SceneLoader;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;
	class Skybox;
	class ParticleSystemImpl;
	class ParticleSystem;
	class CpuParticleSystem;
	class ComputeParticleSystem;
	class TransformFeedbackParticleSystem;
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
	class BillboardBase;
	class BillboardList;

	DECLARE_SMART_PTR( SceneNode );
	DECLARE_SMART_PTR( Scene );
	DECLARE_SMART_PTR( SceneFileContext );
	DECLARE_SMART_PTR( SceneFileParser );
	DECLARE_SMART_PTR( Skybox );
	DECLARE_SMART_PTR( ParticleSystem );
	DECLARE_SMART_PTR( ParticleSystemImpl );
	DECLARE_SMART_PTR( CpuParticleSystem );
	DECLARE_SMART_PTR( TransformFeedbackParticleSystem );
	DECLARE_SMART_PTR( ComputeParticleSystem );
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
	DECLARE_SMART_PTR( BillboardBase );
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
