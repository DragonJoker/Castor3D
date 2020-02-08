/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BONE_H___
#define ___C3D_BONE_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/06/2013
	\version	0.7.0
	\~english
	\brief		Class holding bone data.
	\remark		Holds weight for each vertice and matrix from mesh space to bone space.
	\~french
	\brief		Classe contenant les données d'un bone.
	\remark		Contient les poids pour chaque vertice et la matrice de transformation de l'espace objet vers l'espace bone.
	*/
	class Bone
		: public std::enable_shared_from_this< Bone >
		, public castor::Named
	{
		friend class Skeleton;
		friend class BinaryWriter< Bone >;
		friend class BinaryParser< Bone >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	skeleton	The parent skeleton.
		 *\param[in]	offset		The transfromation matrix from mesh space to bone space.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	skeleton	Le squelette parent.
		 *\param[in]	offset		La matrice de transformation de l'espace objet vers l'espace du bone.
		 */
		C3D_API explicit Bone( Skeleton & skeleton
			, castor::Matrix4x4f const & offset );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Bone();
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere for each bone, for given mesh.
		 *\~french
		 *\brief		Calcule les bounding box et sphere, pour chaque os, pour le maillage donné.
		 */
		C3D_API castor::BoundingBox computeBoundingBox( Mesh const & mesh
			, uint32_t boneIndex )const;
		/**
		 *\~english
		 *\return		The transfromation matrix from mesh space to bone space.
		 *\~french
		 *\return		La matrice de transformation de l'espace objet vers l'espace du bone.
		 */
		inline const castor::Matrix4x4f & getOffsetMatrix()const
		{
			return m_offset;
		}
		/**
		 *\~english
		 *\return		The absolute transfromation matrix from mesh space to bone space.
		 *\~french
		 *\return		La matrice de transformation absolute de l'espace objet vers l'espace du bone.
		 */
		inline const castor::Matrix4x4f & getAbsoluteOffsetMatrix()const
		{
			return m_absoluteOffset;
		}
		/**
		 *\~english
		 *\return		The parent skeleton.
		 *\~french
		 *\return		Le squelette parent.
		 */
		inline const Skeleton & getSkeleton()const
		{
			return m_skeleton;
		}
		/**
		 *\~english
		 *\return		The parent bone.
		 *\~french
		 *\return		L'os parent.
		 */
		BoneSPtr getParent()const
		{
			return m_parent;
		}

	private:
		/**
		 *\~english
		 *\brief		Adds a child bone.
		 *\param[in]	bone	The bone.
		 *\~french
		 *\brief		Ajoute un os enfant.
		 *\param[in]	bone	L'os.
		 */
		void addChild( BoneSPtr bone );
		/**
		 *\~english
		 *\brief		Sets the parent bone.
		 *\param[in]	bone	The bone.
		 *\~french
		 *\brief		Définit l'os parent.
		 *\param[in]	bone	L'os.
		 */
		void setParent( BoneSPtr bone );

	private:
		BoneSPtr m_parent;
		castor::Matrix4x4f m_offset;
		castor::Matrix4x4f m_absoluteOffset;
		BonePtrStrMap m_children;
		Skeleton & m_skeleton;
	};
}

#endif
