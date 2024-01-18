/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_BONE___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_BONE___

#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

namespace castor3d
{
	class SkeletonAnimationInstanceBone
		: public SkeletonAnimationInstanceObject
	{
	public:
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstanceBone && rhs )noexcept = default;
		C3D_API SkeletonAnimationInstanceBone & operator=( SkeletonAnimationInstanceBone && rhs )noexcept = delete;
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstanceBone const & rhs ) = delete;
		C3D_API SkeletonAnimationInstanceBone & operator=( SkeletonAnimationInstanceBone const & rhs ) = delete;
		C3D_API ~SkeletonAnimationInstanceBone()noexcept override = default;
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

	private:
		void doApply()override;

	private:
		//!\~english	The animation bone.
		//!\~french		L'animation d'os.
		SkeletonAnimationBone & m_animationBone;
	};
}

#endif
