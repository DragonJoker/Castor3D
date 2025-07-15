/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BoneNode_H___
#define ___C3D_BoneNode_H___

#include "SkeletonNode.hpp"

namespace c3d
{
	class BoneNode
		: public SkeletonNode
	{
		friend class Skeleton;
		friend class BinaryWriter< BoneNode >;
		friend class BinaryParser< BoneNode >;
		friend class BinaryParser< Skeleton >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name				The bone name.
		 *\param[in]	skeleton			The parent skeleton.
		 *\param[in]	inverseTransform	The transformation matrix from mesh space to bone space.
		 *\param[in]	id					The bone ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name				Le nom de l'os.
		 *\param[in]	skeleton			Le squelette parent.
		 *\param[in]	inverseTransform	La matrice de transformation de l'espace objet vers l'espace du noeud.
		 *\param[in]	id					L'ID de l'os.
		 */
		C3D_API BoneNode( String name
			, Skeleton & skeleton
			, Matrix4x4f inverseTransform
			, uint32_t id );
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere for the bone, for given mesh.
		 *\~french
		 *\brief		Calcule les bounding box et sphere, pour l'os, pour le maillage donné.
		 */
		C3D_API BoundingBox computeBoundingBox( Mesh const & mesh
			, uint32_t boneIndex )const;
		/**
		 *\~english
		 *\return		A clone of this object.
		 *\~french
		 *\return		Un clone de cet objet.
		 */
		C3D_API SkeletonNodeUPtr clone( Skeleton & parent )const override;

		uint32_t getId()const noexcept
		{
			return m_id;
		}

		Matrix4x4f const & getInverseTransform()const noexcept
		{
			return m_inverseTransform;
		}

		void setInverseTransform( Matrix4x4f value )noexcept
		{
			m_inverseTransform = c3d::move( value );
		}

	private:
		uint32_t m_id;
		Matrix4x4f m_inverseTransform;
	};
}

#endif
