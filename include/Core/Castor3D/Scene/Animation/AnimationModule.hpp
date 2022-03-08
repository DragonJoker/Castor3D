/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneAnimationModule_H___
#define ___C3D_SceneAnimationModule_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Animation/AnimationModule.hpp"

namespace castor3d
{
	/**@name Scene */
	//@{
	/**@name Animation */
	//@{

	/**
	*\~english
	*\brief
	*	An animated mesh.
	*\~french
	*\brief
	*	Un maillage animé.
	*/
	class AnimatedMesh;
	/**
	*\~english
	*\brief
	*	Represents the animated objects
	*\~french
	*\brief
	*	Représente les objets animés
	*/
	class AnimatedObject;
	/**
	*\~english
	*\brief
	*	Class which represents the animated object groups
	*\remarks
	*	Animated object groups, are a group of objects sharing a selected list of animations and needing synchronised animating
	*\~french
	*\brief
	*	Représente un groupe d'objets animés
	*\remarks
	*	Un groupe d'objets animés est la liste d'objets partageant les mêmes animations et nécessitant des animations synchronisées
	*/
	class AnimatedObjectGroup;
	/**
	*\~english
	*\brief
	*	An animated scene node.
	*\~french
	*\brief
	*	Un noeud de scène animé.
	*/
	class AnimatedSceneNode;
	/**
	*\~english
	*\brief
	*	An animated skeleton.
	*\~french
	*\brief
	*	Un squelette animé.
	*/
	class AnimatedSkeleton;
	/**
	*\~english
	*\brief
	*	An animated texture.
	*\~french
	*\brief
	*	Une texture animée.
	*/
	class AnimatedTexture;
	/**
	*\~english
	*\brief
	*	Animation instance class.
	*\remarks
	*	Used to play an Animation on a specific object.
	*\~french
	*\brief
	*	Classe d'instance d'animation
	*\remarks
	*	Utilisée pour jouer une animation sur un objet particulier.
	*/
	class AnimationInstance;

	/**@name SceneNode */
	//@{
	class SceneNodeAnimation;
	class SceneNodeAnimationKeyFrame;

	CU_DeclareSmartPtr( SceneNodeAnimation );
	CU_DeclareSmartPtr( SceneNodeAnimationKeyFrame );
	//@}

	struct GroupAnimation
	{
		castor::String name;
		AnimationState state{};
		bool looped{};
		float scale{};
		castor::Milliseconds startingPoint{};
		castor::Milliseconds stoppingPoint{};
	};
	using GroupAnimationMap = std::map< castor::String, GroupAnimation >;

	using Animable = AnimableT< Engine >;
	using Animation = AnimationT< Engine >;

	CU_DeclareSmartPtr( Animable );
	CU_DeclareSmartPtr( AnimatedObjectGroup );
	CU_DeclareSmartPtr( AnimatedMesh );
	CU_DeclareSmartPtr( AnimatedObject );
	CU_DeclareSmartPtr( AnimatedSceneNode );
	CU_DeclareSmartPtr( AnimatedSkeleton );
	CU_DeclareSmartPtr( AnimatedTexture );
	CU_DeclareSmartPtr( Animation );
	CU_DeclareSmartPtr( AnimationInstance );

	//! AnimatedObjectGroup pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimatedObjectGroupSPtr, AnimatedObjectGroupPtrStr );
	//! AnimatedObject pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! Animation pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimationInstanceUPtr, AnimationInstancePtrStr );
	//! Animation pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimationSPtr, AnimationPtrStr );

	using OnAnimatedSkeletonChangeFunction = std::function< void( AnimatedObjectGroup const &, AnimatedSkeleton & ) >;
	using OnAnimatedSkeletonChange = castor::SignalT< OnAnimatedSkeletonChangeFunction >;
	using OnAnimatedSkeletonChangeConnection = OnAnimatedSkeletonChange::connection;

	using OnAnimatedMeshChangeFunction = std::function< void( AnimatedObjectGroup const &, AnimatedMesh & ) >;
	using OnAnimatedMeshChange = castor::SignalT< OnAnimatedMeshChangeFunction >;
	using OnAnimatedMeshChangeConnection = OnAnimatedMeshChange::connection;

	using OnAnimatedTextureChangeFunction = std::function< void( AnimatedObjectGroup const &, AnimatedTexture & ) >;
	using OnAnimatedTextureChange = castor::SignalT< OnAnimatedTextureChangeFunction >;
	using OnAnimatedTextureChangeConnection = OnAnimatedTextureChange::connection;

	using OnAnimatedSceneNodeChangeFunction = std::function< void( AnimatedObjectGroup const &, AnimatedSceneNode & ) >;
	using OnAnimatedSceneNodeChange = castor::SignalT< OnAnimatedSceneNodeChangeFunction >;
	using OnAnimatedSceneNodeChangeConnection = OnAnimatedSceneNodeChange::connection;
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for AnimatedObjectGroup.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour AnimatedObjectGroup.
	*/
	template<>
	struct PtrCacheTraitsT< AnimatedObjectGroup, castor::String >
		: PtrCacheTraitsBaseT< AnimatedObjectGroup, castor::String >
	{
		using ResT = AnimatedObjectGroup;
		using KeyT = castor::String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	using AnimatedObjectGroupCacheTraits = PtrCacheTraitsT< AnimatedObjectGroup, castor::String >;
	using AnimatedObjectGroupCache = castor::ResourceCacheT< AnimatedObjectGroup
		, castor::String
		, AnimatedObjectGroupCacheTraits >;

	using AnimatedObjectGroupRes = AnimatedObjectGroupCacheTraits::ElementPtrT;
	using AnimatedObjectGroupResPtr = AnimatedObjectGroupCacheTraits::ElementObsT;

	CU_DeclareCUSmartPtr( castor3d, AnimatedObjectGroupCache, C3D_API );

	//@}
	//@}
}

namespace castor
{
	template<>
	struct ResourceCacheT< castor3d::AnimatedObjectGroup
		, String
		, castor3d::AnimatedObjectGroupCacheTraits >;
}

#endif
