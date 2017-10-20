/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_NODE___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_NODE___

#include "SkeletonAnimationInstanceObject.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationNode for abstract nodes
	\remark		Used to decompose the model and place intermediate animations
	\~french
	\brief		Implémentation de SkeletonAnimationNode pour des noeuds abstraits.
	\remark		Utilisé afin de décomposer le modèle et ajouter des animations intermédiaires
	*/
	class SkeletonAnimationInstanceNode
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
		C3D_API SkeletonAnimationInstanceNode( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationNode & p_animationObject, SkeletonAnimationInstanceObjectPtrStrMap & p_allObjects );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationInstanceNode();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimationInstanceNode( SkeletonAnimationInstanceNode && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimationInstanceNode & operator=( SkeletonAnimationInstanceNode && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationInstanceNode( SkeletonAnimationInstanceNode const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimationInstanceNode & operator=( SkeletonAnimationInstanceNode const & p_rhs ) = delete;

	private:
		/**
		 *\copydoc		castor3d::SkeletonAnimatedObject::doApply
		 */
		void doApply()override;
	};
}

#endif
