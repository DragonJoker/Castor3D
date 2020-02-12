/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneAnimationSkeletonModule_H___
#define ___C3D_SceneAnimationSkeletonModule_H___

#include "Castor3D/Scene/Animation/AnimationModule.hpp"

namespace castor3d
{
	/**@name Scene */
	//@{
	/**@name Animation */
	//@{
	/**@name Skeleton */
	//@{

	/**
	*\~english
	*\brief
	*	Skeleton animation instance.
	*\~french
	*\brief
	*	Instance d'animation de squelette.
	*/
	class SkeletonAnimationInstance;
	/**
	*\~english
	*\brief
	*	Implementation of SkeletonAnimationObject for Bones.
	*\~french
	*\brief
	*	Implémentation de SkeletonAnimationObject pour les Bones.
	*/
	class SkeletonAnimationInstanceBone;
	/**
	*\~english
	*\brief
	*	The class which manages key frames
	*\remarks
	*	Key frames are the frames where the animation must be at a precise state
	*\~french
	*\brief
	*	Classe qui gère une key frame
	*\remarks
	*	Les key frames sont les frames auxquelles une animation est dans un état précis
	*/
	class SkeletonAnimationInstanceKeyFrame;
	/**
	*\~english
	*\brief
	*	Implementation of SkeletonAnimationNode for abstract nodes
	*\remarks
	*	Used to decompose the model and place intermediate animations
	*\~french
	*\brief
	*	Implémentation de SkeletonAnimationNode pour des noeuds abstraits.
	*\remarks
	*	Utilisé afin de décomposer le modèle et ajouter des animations intermédiaires
	*/
	class SkeletonAnimationInstanceNode;
	/**
	*\~english
	*\brief
	*	Class which represents the moving things.
	*\remarks
	*	Manages translation, scaling, rotation of the thing.
	*\~french
	*\brief
	*	Classe de représentation de choses mouvantes.
	*\remarks
	*	Gère les translations, mises à l'échelle, rotations de la chose.
	*/
	class SkeletonAnimationInstanceObject;

	CU_DeclareSmartPtr( SkeletonAnimationInstance );
	CU_DeclareSmartPtr( SkeletonAnimationInstanceBone );
	CU_DeclareSmartPtr( SkeletonAnimationInstanceNode );
	CU_DeclareSmartPtr( SkeletonAnimationInstanceObject );

	//! MovingObject pointer map, sorted by name.
	CU_DeclareMap( castor::String, SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtrStr );
	//! SkeletonAnimationInstanceObject pointer array.
	CU_DeclareVector( SkeletonAnimationInstanceObjectSPtr, SkeletonAnimationInstanceObjectPtr );
	//! Skeleton animation instance pointer array.
	CU_DeclareVector( std::reference_wrapper< SkeletonAnimationInstance >, SkeletonAnimationInstance );

	//@}
	//@}
	//@}
}

#endif
