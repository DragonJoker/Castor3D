/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_BONE___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_BONE___

#include "SkeletonAnimationInstanceObject.hpp"

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
		 *\brief		Constructor.
		 *\param[in]	p_animationInstance	The parent skeleton animation instance.
		 *\param[in]	p_animationObject	The animation object.
		 *\param[out]	p_allObjects		Receives this object's children.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animationInstance	L'instance d'animation de squelette parent.
		 *\param[in]	p_animationObject	L'animation d'objet.
		 *\param[out]	p_allObjects		Reçoit les enfants de cet objet.
		 */
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationBone & p_animationObject, SkeletonAnimationInstanceObjectPtrStrMap & p_allObjects );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationInstanceBone();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstanceBone && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimationInstanceBone & operator=( SkeletonAnimationInstanceBone && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationInstanceBone( SkeletonAnimationInstanceBone const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimationInstanceBone & operator=( SkeletonAnimationInstanceBone const & p_rhs ) = delete;

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
