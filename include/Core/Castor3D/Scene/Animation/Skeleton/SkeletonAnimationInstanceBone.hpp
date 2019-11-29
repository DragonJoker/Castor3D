/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_BONE___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_BONE___

#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationObject for Bones.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour les Bones.
	*/
	class SkeletonAnimationInstanceBone
		: public SkeletonAnimationInstanceObject
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstanceBone && rhs ) = default;
		C3D_API SkeletonAnimationInstanceBone & operator=( SkeletonAnimationInstanceBone && rhs ) = delete;
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstanceBone const & rhs ) = delete;
		C3D_API SkeletonAnimationInstanceBone & operator=( SkeletonAnimationInstanceBone const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animationInstance	The parent skeleton animation instance.
		 *\param[in]	animationObject		The animation object.
		 *\param[out]	allObjects			Receives this object's children.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animationInstance	L'instance d'animation de squelette parent.
		 *\param[in]	animationObject		L'animation d'objet.
		 *\param[out]	allObjects			Reçoit les enfants de cet objet.
		 */
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstance & animationInstance
			, SkeletonAnimationBone & animationObject
			, SkeletonAnimationInstanceObjectPtrArray & allObjects );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationInstanceBone();

	private:
		/**
		 *\copydoc		castor3d::SkeletonAnimatedObject::doApply
		 */
		void doApply()override;

	private:
		//!\~english	The animation bone.
		//!\~french		L'animation d'os.
		SkeletonAnimationBone & m_animationBone;
	};
}

#endif
