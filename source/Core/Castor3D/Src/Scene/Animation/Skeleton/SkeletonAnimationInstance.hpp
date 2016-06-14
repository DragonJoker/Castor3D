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
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_H___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_H___

#include "Scene/Animation/AnimationInstance.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\todo		Write and Read functions.
	\~english
	\brief		Skeleton animation instance.
	\~french
	\brief		Instance d'animation de squelette.
	*/
	class SkeletonAnimationInstance
		: public AnimationInstance
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_object	The parent AnimatedSkeleton.
		 *\param[in]	p_animation	The animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_object	L'AnimatedSkeleton parent.
		 *\param[in]	p_animation	L'animation.
		 */
		C3D_API SkeletonAnimationInstance( AnimatedSkeleton & p_object, SkeletonAnimation const & p_animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SkeletonAnimationInstance();
		/**
		 *\~english
		 *\brief		Retrieves an animated bone.
		 *\param[in]	p_bone	The bone.
		 *\~french
		 *\brief		Récupère un os animé.
		 *\param[in]	p_bone	L'os.
		 */
		C3D_API SkeletonAnimationInstanceObjectSPtr GetObject( Bone const & p_bone )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated node.
		 *\param[in]	p_name	The node name.
		 *\~french
		 *\brief		Récupère un noeud animé.
		 *\param[in]	p_name	Le nom du noeud.
		 */
		C3D_API SkeletonAnimationInstanceObjectSPtr GetObject( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object.
		 *\param[in]	p_type	The object type.
		 *\param[in]	p_name	The object name.
		 *\~french
		 *\brief		Récupère un objet animé.
		 *\param[in]	p_type	Le type de l'objet.
		 *\param[in]	p_name	Le nom de l'objet.
		 */
		C3D_API SkeletonAnimationInstanceObjectSPtr GetObject( SkeletonAnimationObjectType p_type, Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Sets the key frames interpolator type.
		 *\param[in]	p_type	The interpolator type.
		 *\~french
		 *\return		Définit le type d'interpolateur des key frames.
		 *\param[in]	p_type	Le type d'interpolateur.
		 */
		C3D_API void SetInterpolatorType( InterpolatorType p_type );
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le nombre d'objets.
		 */
		inline auto GetObjectsCount()const
		{
			return m_toMove.size();
		}
		/**
		 *\~english
		 *\return		The root objects count.
		 *\~french
		 *\return		Le nombre d'objets racines.
		 */
		inline auto GetRootObjectsCount()const
		{
			return m_arrayMoving.size();
		}
		/**
		 *\~english
		 *\return		The root objects beginning iterator.
		 *\~french
		 *\return		L'itérateur sur le début des objets racines.
		 */
		inline auto begin()
		{
			return m_arrayMoving.begin();
		}
		/**
		 *\~english
		 *\return		The root objects beginning iterator.
		 *\~french
		 *\return		L'itérateur sur le début des objets racines.
		 */
		inline auto begin()const
		{
			return m_arrayMoving.begin();
		}
		/**
		 *\~english
		 *\return		The root objects end iterator.
		 *\~french
		 *\return		L'itérateur sur la fin des objets racines.
		 */
		inline auto end()
		{
			return m_arrayMoving.end();
		}
		/**
		 *\~english
		 *\return		The root objects end iterator.
		 *\~french
		 *\return		L'itérateur sur la fin des objets racines.
		 */
		inline auto end()const
		{
			return m_arrayMoving.end();
		}

	private:
		/**
		 *\~copydoc		Casto3D::AnimationInstance::DoUpdate
		 */
		void DoUpdate()override;

	protected:
		//!\~english	The root moving objects.
		//!\~french		Les objets mouvants racine.
		SkeletonAnimationInstanceObjectPtrArray m_arrayMoving;
		//!\~english	The moving objects.
		//!\~french		Les objets mouvants.
		SkeletonAnimationInstanceObjectPtrStrMap m_toMove;

		friend class BinaryWriter< SkeletonAnimation >;
		friend class BinaryParser< SkeletonAnimation >;
	};
}

#endif
