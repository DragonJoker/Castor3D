/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_H___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_H___

#include "Castor3D/Scene/Animation/AnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceKeyFrame.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
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
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API SkeletonAnimationInstance( SkeletonAnimationInstance && rhs ) = default;
		C3D_API SkeletonAnimationInstance & operator=( SkeletonAnimationInstance && rhs ) = delete;
		C3D_API SkeletonAnimationInstance( SkeletonAnimationInstance const & rhs ) = delete;
		C3D_API SkeletonAnimationInstance & operator=( SkeletonAnimationInstance const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	object		The parent AnimatedSkeleton.
		 *\param[in]	animation	The animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	object		L'AnimatedSkeleton parent.
		 *\param[in]	animation	L'animation.
		 */
		C3D_API SkeletonAnimationInstance( AnimatedSkeleton & object, SkeletonAnimation & animation );
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
		 *\param[in]	bone	The bone.
		 *\~french
		 *\brief		Récupère un os animé.
		 *\param[in]	bone	L'os.
		 */
		C3D_API SkeletonAnimationInstanceObjectSPtr getObject( Bone const & bone )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated node.
		 *\param[in]	name	The node name.
		 *\~french
		 *\brief		Récupère un noeud animé.
		 *\param[in]	name	Le nom du noeud.
		 */
		C3D_API SkeletonAnimationInstanceObjectSPtr getObject( castor::String const & name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object.
		 *\param[in]	type	The object type.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Récupère un objet animé.
		 *\param[in]	type	Le type de l'objet.
		 *\param[in]	name	Le nom de l'objet.
		 */
		C3D_API SkeletonAnimationInstanceObjectSPtr getObject( SkeletonAnimationObjectType type
			, castor::String const & name )const;
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
		 *\return		The root objects beginning iterator.
		 *\~french
		 *\return		L'itérateur sur le début des objets racines.
		 */
		inline auto begin()
		{
			return m_toMove.begin();
		}
		/**
		 *\~english
		 *\return		The root objects beginning iterator.
		 *\~french
		 *\return		L'itérateur sur le début des objets racines.
		 */
		inline auto begin()const
		{
			return m_toMove.begin();
		}
		/**
		 *\~english
		 *\return		The root objects end iterator.
		 *\~french
		 *\return		L'itérateur sur la fin des objets racines.
		 */
		inline auto end()
		{
			return m_toMove.end();
		}
		/**
		 *\~english
		 *\return		The root objects end iterator.
		 *\~french
		 *\return		L'itérateur sur la fin des objets racines.
		 */
		inline auto end()const
		{
			return m_toMove.end();
		}

	private:
		/**
		 *\copydoc		Casto3D::AnimationInstance::doUpdate
		 */
		void doUpdate()override;

	protected:
		//!\~english	The moving objects.
		//!\~french		Les objets mouvants.
		SkeletonAnimationInstanceObjectPtrArray m_toMove;
		//!\~english	The instance keyframes.
		//!\~french		Les instances des keyframes.
		SkeletonAnimationInstanceKeyFrameArray m_keyFrames;
		//!\~english	Iterator to the current keyframe (when playing the animation).
		//!\~french		Itérateur sur la keyframe courante (quand l'animation est jouée).
		SkeletonAnimationInstanceKeyFrameArray::iterator m_curr;
	};
}

#endif
