/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_H___
#define ___C3D_SKELETON_H___

#include "Castor3DPrerequisites.hpp"

#include "Animation/Animable.hpp"
#include "Bone.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\~english
	\brief		The skeleton, holds each bone
	\~french
	\brief		Le squelette, contient chaque bone
	*/
	class Skeleton
		: public Animable
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API Skeleton( Skeleton && rhs ) = default;
		C3D_API Skeleton & operator=( Skeleton && rhs ) = default;
		C3D_API Skeleton( Skeleton const & rhs ) = delete;
		C3D_API Skeleton & operator=( Skeleton const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène.
		 */
		C3D_API explicit Skeleton( Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Skeleton();
		/**
		 *\~english
		 *\brief		Creates a bone.
		 *\param[in]	name	The bone name.
		 *\param[in]	offset	The transfromation matrix from mesh space to bone space.
		 *\~french
		 *\brief		Crée un os.
		 *\param[in]	name	Le nom de l'os.
		 *\param[in]	offset	La matrice de transformation de l'espace objet vers l'espace du bone.
		 */
		C3D_API BoneSPtr createBone( castor::String const & name
			, castor::Matrix4x4r const & offset );
		/**
		 *\~english
		 *\brief		Finds a bone from a name.
		 *\param[in]	name	The bone name.
		 *\~french
		 *\brief		Trouve un os à partir de son nom.
		 *\param[in]	name	Le nom de l'os.
		 */
		C3D_API BoneSPtr findBone( castor::String const & name )const;
		/**
		 *\~english
		 *\brief		adds a bone to another bone's children
		 *\param[in]	bone	The bone.
		 *\param[in]	parent	The parent bone.
		 *\~french
		 *\brief		Ajoute un os aux enfants d'un autre os.
		 *\param[in]	bone	L'os.
		 *\param[in]	parent	L'os parent.
		 */
		C3D_API void setBoneParent( BoneSPtr bone, BoneSPtr parent );
		/**
		 *\~english
		 *\brief		Creates an animation
		 *\param[in]	name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Crée une animation
		 *\param[in]	name	Le nom de l'animation
		 *\return		l'animation
		 */
		C3D_API SkeletonAnimation & createAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere for each bone, for given mesh.
		 *\~french
		 *\brief		Calcule les bounding box et sphere, pour chaque os, pour le maillage donné.
		 */
		C3D_API void computeContainers( Mesh & mesh );
		/**
		 *\~english
		 *\return		The global inverse transform.
		 *\~french
		 *\return		La transformation globale inversée.
		 */
		inline castor::Matrix4x4r const & getGlobalInverseTransform()const
		{
			return m_globalInverse;
		}
		/**
		 *\~english
		 *\brief		Sets the global inverse transform.
		 *\param[in]	transform	The new value.
		 *\~french
		 *\brief		Définit la transformation globale inversée.
		 *\param[in]	transform	La nouvelle valeur.
		 */
		inline void setGlobalInverseTransform( castor::Matrix4x4r const & transform )
		{
			m_globalInverse = transform;
		}
		/**
		 *\~english
		 *\return		The bones count.
		 *\~french
		 *\return		Le nombre d'os.
		 */
		inline size_t getBonesCount()const
		{
			return m_bones.size();
		}
		/**
		 *\~english
		 *\return		An iterator to the first bone.
		 *\~french
		 *\return		Un itérateur sur le premier os.
		 */
		inline auto begin()
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the first bone.
		 *\~french
		 *\return		Un itérateur sur le premier os.
		 */
		inline auto begin()const
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the bones array.
		 *\~french
		 *\return		Un itérateur sur la fin du tableau d'os.
		 */
		inline auto end()
		{
			return m_bones.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the bones array.
		 *\~french
		 *\return		Un itérateur sur la fin du tableau d'os.
		 */
		inline auto end()const
		{
			return m_bones.end();
		}
		/**
		 *\~english
		 *\return		The bones boxes.
		 *\~french
		 *\return		Les boîtes des os.
		 */
		inline std::vector< castor::BoundingBox > const & getContainers( Mesh & mesh )const
		{
			auto it = m_boxes.find( &mesh );

			if ( it != m_boxes.end() )
			{
				return it->second;
			}

			static std::vector< castor::BoundingBox > const dummy;
			return dummy;
		}

	private:
		//!\~english	The bones.
		//!\~french		Les bones.
		BonePtrArray m_bones;
		//!\~english	The global skeleton transform.
		//!\~french		La transformation globale du squelette.
		castor::Matrix4x4r m_globalInverse;
		//!\~english	The bounding box for each bone, sorted by mesh.
		//!\~french		La bounding box pour chaque os, trié par maillage.
		std::map< Mesh *, std::vector< castor::BoundingBox > > m_boxes;

		friend class BinaryWriter< Skeleton >;
		friend class BinaryParser< Skeleton >;
	};
}

#endif
