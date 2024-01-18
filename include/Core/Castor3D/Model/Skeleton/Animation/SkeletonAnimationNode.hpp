/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationNode_H___
#define ___C3D_SkeletonAnimationNode_H___

#include "Castor3D/Binary/BinaryModule.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp"

namespace castor3d
{
	class SkeletonAnimationNode
		: public SkeletonAnimationObject
	{
	public:
		C3D_API SkeletonAnimationNode( SkeletonAnimationNode && rhs )noexcept = default;
		C3D_API SkeletonAnimationNode & operator=( SkeletonAnimationNode && rhs ) = delete;
		C3D_API SkeletonAnimationNode( SkeletonAnimationNode const & rhs ) = delete;
		C3D_API SkeletonAnimationNode & operator=( SkeletonAnimationNode const & rhs )noexcept = delete;
		C3D_API ~SkeletonAnimationNode()noexcept override = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 */
		C3D_API explicit SkeletonAnimationNode( SkeletonAnimation & animation );
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		C3D_API castor::String const & getName()const override;
		/**
		 *\~english
		 *\return		The object's node transform.
		 *\~french
		 *\return		La transformation de noeud de l'objet.
		 */
		C3D_API NodeTransform const & getNodeTransform()const noexcept override;

		void setNode( SkeletonNode & node )
		{
			m_node = &node;
		}

		SkeletonNode * getNode()const
		{
			return m_node;
		}

	private:
		//!\~english	The node.
		//!\~french		Le noeud.
		SkeletonNode * m_node{};

		friend class BinaryWriter< SkeletonAnimationNode >;
		friend class BinaryParser< SkeletonAnimationNode >;
	};
}

#endif
