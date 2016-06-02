/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_NODE___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_NODE___

#include "SkeletonAnimationInstanceObject.hpp"

namespace Castor3D
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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animationInstance	L'instance d'animation de squelette parent.
		 *\param[in]	p_animationObject	L'animation d'objet.
		 */
		C3D_API SkeletonAnimationInstanceNode( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationNode & p_animationObject );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationInstanceNode();

	private:
		/**
		 *\~copydoc		Castor3D::SkeletonAnimatedObject::DoApply
		 */
		void DoApply()override;
	};
}

#endif
