/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelSkeletonAnimationModule_H___
#define ___C3D_ModelSkeletonAnimationModule_H___

#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Animation */
	//@{
	/**@name Skeleton */
	//@{

	/**
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the moving things.
	\remark		Manages translation, scaling, rotation of the thing.
	\~french
	\brief		Classe de représentation de choses mouvantes.
	\remark		Gère les translations, mises à l'échelle, rotations de la chose.
	*/
	enum class SkeletonAnimationObjectType
		: uint8_t
	{
		//!\~english	Node objects.
		//!\~french		Objets noeud.
		eNode,
		//!\~english	Bone objects.
		//!\~french		Objets os.
		eBone,
		CU_ScopedEnumBounds( eNode )
	};
	castor::String getName( SkeletonAnimationObjectType value );
	/**
	\~english
	\brief		Skeleton animation class.
	\~french
	\brief		Classe d'animation de squelette.
	*/
	class SkeletonAnimation;
	/**
	\~english
	\brief		Implementation of SkeletonAnimationObject for Bones.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour les Bones.
	*/
	class SkeletonAnimationBone;
	/**
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The class which manages key frames
	\remark		Key frames are the frames where the animation must be at a precise state
	\~french
	\brief		Classe qui gère une key frame
	\remark		Les key frames sont les frames auxquelles une animation est dans un état précis
	*/
	class SkeletonAnimationKeyFrame;
	/**
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationObject for abstract nodes
	\remark		Used to decompose the model and place intermediate animation nodes.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour des noeuds abstraits.
	\remark		Utilisé afin de décomposer le modèle et ajouter des noeuds d'animation intermédiaires.
	*/
	class SkeletonAnimationNode;
	/**
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the skeleton animation objects.
	\remark		Manages translation, scaling, rotation of the object.
	\~french
	\brief		Classe de représentation d'objets d'animations de squelette.
	\remark		Gère les translations, mises à l'échelle, rotations de l'objet.
	*/
	class SkeletonAnimationObject;

	using ObjectTransform = std::pair< SkeletonAnimationObject *, castor::Matrix4x4f >;
	using TransformArray = std::vector< ObjectTransform >;

	CU_DeclareSmartPtr( SkeletonAnimation );
	CU_DeclareSmartPtr( SkeletonAnimationKeyFrame );
	CU_DeclareSmartPtr( SkeletonAnimationObject );
	CU_DeclareSmartPtr( SkeletonAnimationBone );
	CU_DeclareSmartPtr( SkeletonAnimationNode );

	//! SkeletonAnimationObject pointer map, sorted by name.
	CU_DeclareMap( castor::String, SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtrStr );
	//! SkeletonAnimationObject pointer array.
	CU_DeclareVector( SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtr );

	//@}
	//@}
	//@}
}

#endif
