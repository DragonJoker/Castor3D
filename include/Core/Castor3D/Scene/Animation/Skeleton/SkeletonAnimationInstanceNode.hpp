/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_NODE___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_NODE___

#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

namespace castor3d
{
	class SkeletonAnimationInstanceNode
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
		C3D_API SkeletonAnimationInstanceNode( SkeletonAnimationInstanceNode && rhs ) = default;
		C3D_API SkeletonAnimationInstanceNode & operator=( SkeletonAnimationInstanceNode && rhs ) = delete;
		C3D_API SkeletonAnimationInstanceNode( SkeletonAnimationInstanceNode const & rhs ) = delete;
		C3D_API SkeletonAnimationInstanceNode & operator=( SkeletonAnimationInstanceNode const & rhs ) = delete;
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
		C3D_API SkeletonAnimationInstanceNode( SkeletonAnimationInstance & animationInstance
			, SkeletonAnimationNode & animationObject
			, SkeletonAnimationInstanceObjectPtrArray & allObjects );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationInstanceNode();

	private:
		/**
		 *\copydoc		castor3d::SkeletonAnimatedObject::doApply
		 */
		void doApply()override;
	};
}

#endif
