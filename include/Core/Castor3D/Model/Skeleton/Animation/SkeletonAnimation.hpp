/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimation_H___
#define ___C3D_SkeletonAnimation_H___

#include "SkeletonAnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Animation/Animation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

namespace castor3d
{
	class SkeletonAnimation
		: public Animation
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animable	The parent animable object.
		 *\param[in]	name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animable	L'objet animable parent.
		 *\param[in]	name		Le nom de l'animation.
		 */
		C3D_API SkeletonAnimation( Animable & animable
			, castor::String const & name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SkeletonAnimation();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimation( SkeletonAnimation && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimation & operator=( SkeletonAnimation && rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimation( SkeletonAnimation const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimation & operator=( SkeletonAnimation const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Creates and adds a moving node.
		 *\param[in]	name	The node name.
		 *\param[in]	parent	The moving object's parent.
		 *\~french
		 *\brief		Crée et ajoute un noeud mouvant.
		 *\param[in]	name	Le nom du noeud.
		 *\param[in]	parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr addObject( castor::String const & name
			, SkeletonAnimationObjectSPtr parent );
		/**
		 *\~english
		 *\brief		Creates and adds a moving bone.
		 *\param[in]	bone	The bone to add.
		 *\param[in]	parent	The moving object's parent.
		 *\~french
		 *\brief		Crée et ajoute un os mouvant.
		 *\param[in]	bone	L'os.
		 *\param[in]	parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr addObject( BoneSPtr bone
			, SkeletonAnimationObjectSPtr parent );
		/**
		 *\~english
		 *\brief		adds an animated object.
		 *\param[in]	object	The animated object to add.
		 *\param[in]	parent	The moving object's parent.
		 *\~french
		 *\brief		Ajoute un objet animé.
		 *\param[in]	object	L'objet animé.
		 *\param[in]	parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr addObject( SkeletonAnimationObjectSPtr object
			, SkeletonAnimationObjectSPtr parent );
		/**
		 *\~english
		 *\brief		Tells if the animation has the animated object.
		 *\param[in]	type	The object type.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Dit si l'animation a l'objet animé.
		 *\param[in]	type	Le type de l'objet.
		 *\param[in]	name	Le nom de l'objet.
		 */
		C3D_API bool hasObject( SkeletonAnimationObjectType type
			, castor::String const & name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated bone.
		 *\param[in]	bone	The bone.
		 *\~french
		 *\brief		Récupère un os animé.
		 *\param[in]	bone	L'os.
		 */
		C3D_API SkeletonAnimationObjectSPtr getObject( Bone const & bone )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Récupère un objet animé.
		 *\param[in]	name	Le nom de l'objet.
		 */
		C3D_API SkeletonAnimationObjectSPtr getObject( castor::String const & name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object.
		 *\param[in]	type	The object type.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Récupère un objet animé.
		 *\param[in]	type	Le type d'objet.
		 *\param[in]	name	Le nom de l'objet.
		 */
		C3D_API SkeletonAnimationObjectSPtr getObject( SkeletonAnimationObjectType type
			, castor::String const & name )const;
		/**
		 *\~english
		 *\return		The moving objects.
		 *\~french
		 *\return		Les objets mouvants.
		 */
		inline SkeletonAnimationObjectPtrStrMap const & getObjects()const
		{
			return m_toMove;
		}
		/**
		 *\~english
		 *\return		The root moving objects.
		 *\~french
		 *\brief		Récupère le nombre d'objets mouvants
		 *\return		Les objets mouvants racines.
		 */
		inline SkeletonAnimationObjectPtrArray const & getRootObjects()const
		{
			return m_arrayMoving;
		}

	protected:
		//!\~english	The root moving objects.
		//!\~french		Les objets mouvants racine.
		SkeletonAnimationObjectPtrArray m_arrayMoving;
		//!\~english	The moving objects.
		//!\~french		Les objets mouvants.
		SkeletonAnimationObjectPtrStrMap m_toMove;

		friend class BinaryWriter< SkeletonAnimation >;
		friend class BinaryParser< SkeletonAnimation >;
		friend class SkeletonAnimationInstance;
	};
}

#endif
