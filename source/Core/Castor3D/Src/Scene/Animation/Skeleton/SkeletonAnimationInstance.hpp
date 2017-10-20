/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_H___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_H___

#include "Scene/Animation/AnimationInstance.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\todo		write and read functions.
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
		C3D_API SkeletonAnimationInstanceObjectSPtr getObject( Bone const & p_bone )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated node.
		 *\param[in]	p_name	The node name.
		 *\~french
		 *\brief		Récupère un noeud animé.
		 *\param[in]	p_name	Le nom du noeud.
		 */
		C3D_API SkeletonAnimationInstanceObjectSPtr getObject( castor::String const & p_name )const;
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
		C3D_API SkeletonAnimationInstanceObjectSPtr getObject( SkeletonAnimationObjectType p_type, castor::String const & p_name )const;
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le nombre d'objets.
		 */
		inline auto getObjectsCount()const
		{
			return m_toMove.size();
		}
		/**
		 *\~english
		 *\return		The root objects count.
		 *\~french
		 *\return		Le nombre d'objets racines.
		 */
		inline auto getRootObjectsCount()const
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
		 *\copydoc		Casto3D::AnimationInstance::doUpdate
		 */
		void doUpdate()override;

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
