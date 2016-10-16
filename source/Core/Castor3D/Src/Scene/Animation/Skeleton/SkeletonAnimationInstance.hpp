/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimationInstance( SkeletonAnimationInstance && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimationInstance & operator=( SkeletonAnimationInstance && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationInstance( SkeletonAnimationInstance const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimationInstance & operator=( SkeletonAnimationInstance const & p_rhs ) = delete;
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
		 *\copydoc		Casto3D::AnimationInstance::DoUpdate
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
