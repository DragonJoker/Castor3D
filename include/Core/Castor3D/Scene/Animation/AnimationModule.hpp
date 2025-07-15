/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneAnimationModule_H___
#define ___C3D_SceneAnimationModule_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Animation/AnimationModule.hpp"

namespace c3d
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

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, SceneNodeAnimation, C3D_API );
	CU_DeclareSmartPtr( c3d, SceneNodeAnimationKeyFrame, C3D_API );
	/** @endcond */

	//@}

	struct GroupAnimation
	{
		explicit GroupAnimation( String name )
			: name{ c3d::move( name ) }
		{
		}

		GroupAnimation( String name
			, AnimationState state
			, bool looped = {}
			, float scale = {}
			, Milliseconds startingPoint = {}
			, Milliseconds stoppingPoint = {}
			, InterpolatorType interpolation = InterpolatorType::eLinear )
			: name{ c3d::move( name ) }
			, state{ state }
			, looped{ looped }
			, scale{ scale }
			, startingPoint{ startingPoint }
			, stoppingPoint{ stoppingPoint }
			, interpolation{ interpolation }
		{
		}

		String name;
		AnimationState state{};
		bool looped{};
		float scale{};
		Milliseconds startingPoint{};
		Milliseconds stoppingPoint{};
		InterpolatorType interpolation{ InterpolatorType::eLinear };
		Milliseconds totalTime{};
	};
	using GroupAnimationMap = StringMap< GroupAnimation >;

	using OnAnimatedSkeletonChangeFunction = Function< void( AnimatedObjectGroup const &, AnimatedSkeleton & ) >;
	using OnAnimatedSkeletonChange = SignalT< OnAnimatedSkeletonChangeFunction >;
	using OnAnimatedSkeletonChangeConnection = OnAnimatedSkeletonChange::connection;

	using OnAnimatedMeshChangeFunction = Function< void( AnimatedObjectGroup const &, AnimatedMesh & ) >;
	using OnAnimatedMeshChange = SignalT< OnAnimatedMeshChangeFunction >;
	using OnAnimatedMeshChangeConnection = OnAnimatedMeshChange::connection;

	using OnAnimatedTextureChangeFunction = Function< void( AnimatedObjectGroup const &, AnimatedTexture & ) >;
	using OnAnimatedTextureChange = SignalT< OnAnimatedTextureChangeFunction >;
	using OnAnimatedTextureChangeConnection = OnAnimatedTextureChange::connection;

	using OnAnimatedSceneNodeChangeFunction = Function< void( AnimatedObjectGroup const &, AnimatedSceneNode & ) >;
	using OnAnimatedSceneNodeChange = SignalT< OnAnimatedSceneNodeChangeFunction >;
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
	struct PtrCacheTraitsT< AnimatedObjectGroup, String >
		: PtrCacheTraitsBaseT< AnimatedObjectGroup, String >
	{
		using ResT = AnimatedObjectGroup;
		using KeyT = String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;
	};

	using AnimatedObjectGroupCacheTraits = PtrCacheTraitsT< AnimatedObjectGroup, String >;
	using AnimatedObjectGroupCache = ResourceCacheT< AnimatedObjectGroup
		, String
		, AnimatedObjectGroupCacheTraits >;

	using AnimatedObjectGroupRes = AnimatedObjectGroupCacheTraits::ElementPtrT;
	using AnimatedObjectGroupResPtr = AnimatedObjectGroupCacheTraits::ElementObsT;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, AnimatedObjectGroup, C3D_API );
	CU_DeclareSmartPtr( c3d, AnimatedObjectGroupCache, C3D_API );
	CU_DeclareSmartPtr( c3d, AnimatedMesh, C3D_API );
	CU_DeclareSmartPtr( c3d, AnimatedObject, C3D_API );
	CU_DeclareSmartPtr( c3d, AnimatedSceneNode, C3D_API );
	CU_DeclareSmartPtr( c3d, AnimatedSkeleton, C3D_API );
	CU_DeclareSmartPtr( c3d, AnimatedTexture, C3D_API );
	CU_DeclareSmartPtr( c3d, AnimationInstance, C3D_API );
	/** @endcond */

	//@}
	//@}
}

namespace c3d
{
	template<>
	struct ResourceCacheT< AnimatedObjectGroup
		, String
		, AnimatedObjectGroupCacheTraits >;
}

#endif
