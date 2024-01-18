/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonNode_H___
#define ___C3D_SkeletonNode_H___

#include "SkeletonModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class SkeletonNode
		: public castor::Named
	{
		friend class Skeleton;
		friend class BinaryWriter< SkeletonNode >;
		friend class BinaryParser< SkeletonNode >;
		friend class BinaryWriter< BoneNode >;
		friend class BinaryParser< BoneNode >;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type				The node type.
		 *\param[in]	name				The node name.
		 *\param[in]	skeleton			The parent skeleton.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type				Le type de noeud.
		 *\param[in]	name				Le nom du noeud.
		 *\param[in]	skeleton			Le squelette parent.
		 */
		C3D_API SkeletonNode( SkeletonNodeType type
			, castor::String name
			, Skeleton & skeleton );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name				The node name.
		 *\param[in]	skeleton			The parent skeleton.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name				Le nom du noeud.
		 *\param[in]	skeleton			Le squelette parent.
		 */
		C3D_API SkeletonNode( castor::String name
			, Skeleton & skeleton );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SkeletonNode()noexcept = default;

		const Skeleton & getSkeleton()const noexcept
		{
			return m_skeleton;
		}

		SkeletonNode * getParent()const noexcept
		{
			return m_parent;
		}

		SkeletonNodeType getType()const noexcept
		{
			return m_type;
		}

		NodeTransform const & getTransform()const noexcept
		{
			return m_transform;
		}

		void setTransform( NodeTransform tranform )noexcept
		{
			m_transform = std::move( tranform );
		}

	private:
		/**
		 *\~english
		 *\brief		Adds a child node.
		 *\param[in]	node	The node.
		 *\~french
		 *\brief		Ajoute un noeud enfant.
		 *\param[in]	node	Le noeud.
		 */
		void addChild( SkeletonNode & node );
		/**
		 *\~english
		 *\brief		Sets the parent node.
		 *\param[in]	node	The node.
		 *\~french
		 *\brief		Définit le noeud parent.
		 *\param[in]	node	Le noeud.
		 */
		void setParent( SkeletonNode & node );

	private:
		SkeletonNodeType m_type;
		Skeleton & m_skeleton;
		SkeletonNode * m_parent{};
		NodeTransform m_transform{};
		castor::StringMap< SkeletonNode * > m_children;
	};
}

#endif
