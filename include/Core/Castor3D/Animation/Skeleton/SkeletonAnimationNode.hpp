/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_NODE___
#define ___C3D_SKELETON_ANIMATION_NODE___

#include "Castor3D/Animation/Skeleton/SkeletonAnimationObject.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationObject for abstract nodes
	\remark		Used to decompose the model and place intermediate animation nodes.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour des noeuds abstraits.
	\remark		Utilisé afin de décomposer le modèle et ajouter des noeuds d'animation intermédiaires.
	*/
	class SkeletonAnimationNode
		: public SkeletonAnimationObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\param[in]	name		The node name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 *\param[in]	name		Le nom du noeud.
		 */
		C3D_API SkeletonAnimationNode( SkeletonAnimation & animation
			, castor::String const & name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SkeletonAnimationNode();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimationNode( SkeletonAnimationNode && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimationNode & operator=( SkeletonAnimationNode && rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationNode( SkeletonAnimationNode const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimationNode & operator=( SkeletonAnimationNode const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		C3D_API virtual castor::String const & getName()const
		{
			return m_name;
		}

	private:
		//!\~english	The node name.
		//!\~french		Le nom du noeud.
		castor::String m_name;

		friend class BinaryWriter< SkeletonAnimationNode >;
		friend class BinaryParser< SkeletonAnimationNode >;
	};
}

#endif
