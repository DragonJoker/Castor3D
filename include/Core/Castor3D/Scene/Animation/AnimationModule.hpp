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

	CU_DeclareSmartPtr( castor3d, SceneNodeAnimation, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneNodeAnimationKeyFrame, C3D_API );
	//@}

	struct GroupAnimation
	{
		castor::String name;
		AnimationState state{};
		bool looped{};
		float scale{};
		castor::Milliseconds startingPoint{};
		castor::Milliseconds stoppingPoint{};
		InterpolatorType interpolation{ InterpolatorType::eLinear };
	};
	using GroupAnimationMap = castor::StringMap< GroupAnimation >;

	using Animable = AnimableT< Engine >;
	using Animation = AnimationT< Engine >;

	CU_DeclareSmartPtr( castor3d, Animable, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimatedObjectGroup, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimatedMesh, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimatedObject, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimatedSceneNode, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimatedSkeleton, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimatedTexture, C3D_API );
	CU_DeclareSmartPtr( castor3d, Animation, C3D_API );
	CU_DeclareSmartPtr( castor3d, AnimationInstance, C3D_API );

	CU_DeclareTemplateSmartPtr( castor3d, Animable );

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

	CU_DeclareSmartPtr( castor3d, AnimatedObjectGroupCache, C3D_API );

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
