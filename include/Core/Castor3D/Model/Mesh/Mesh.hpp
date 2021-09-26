/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Mesh_H___
#define ___C3D_Mesh_H___

#include "MeshModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Animation/Animable.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>

namespace castor3d
{
	class Mesh
		: public castor::Named
		, public Animable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	This mesh name.
		 *\param[in]	scene	The parent scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Le nom du maillage.
		 *\param[in]	scene	La scèene parente.
		 */
		C3D_API Mesh( castor::String const & name
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Mesh()override;
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API Mesh( Mesh && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API Mesh & operator=( Mesh && rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API Mesh( Mesh const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API Mesh & operator=( Mesh const & rhs ) = delete;
		C3D_API void initialise();
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere from the submeshes.
		 *\~french
		 *\brief		Calcule les bounding box et sphere depuis les sous-maillages.
		 */
		C3D_API void updateContainers();
		/**
		 *\~english
		 *\brief		Computes the bounding box and sphere.
		 *\~french
		 *\brief		Calcule les bounding box et sphere.
		 */
		C3D_API void computeContainers();
		/**
		 *\~english
		 *\brief		Returns the total faces count
		 *\return		The faces count
		 *\~french
		 *\brief		Récupère le nombre total de faces
		 *\return		Le compte
		 */
		C3D_API uint32_t getFaceCount()const;
		/**
		 *\~english
		 *\brief		Returns the total vertex count
		 *\return		The vertex count
		 *\~french
		 *\brief		Récupère le nombre total de sommets
		 *\return		Le compte
		 */
		C3D_API uint32_t getVertexCount()const;
		/**
		 *\~english
		 *\brief		Tries to retrieve the submesh at the given index
		 *\param[in]	index	The wanted submesh index
		 *\return		The found submesh, nullptr if not found
		 *\~french
		 *\brief		Essaie de récupérer le sous maillage à l'indice donné
		 *\param[in]	index	L'indice du sous maillage voulu
		 *\return		Le sous maillage trouvé, nullptr si non trouvé
		 */
		C3D_API SubmeshSPtr getSubmesh( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Creates a submesh
		 *\return		The created submesh
		 *\~french
		 *\brief		Crée un sous maillage
		 *\return		Le sous maillage créé
		 */
		C3D_API SubmeshSPtr createSubmesh();
		/**
		 *\~english
		 *\brief		Deletes a given submesh if it is in the mesh's submeshes
		 *\param[in]	submesh	The submesh to delete
		 *\~french
		 *\brief		Supprime le submesh s'il est dans les submesh du mesh
		 *\param[in]	submesh	Le submesh à supprimer
		 */
		C3D_API void deleteSubmesh( SubmeshSPtr & submesh );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		C3D_API void computeNormals( bool reverted = false );
		/**
		 *\~english
		 *\brief		Sets the skeleton
		 *\param[in]	skeleton	The new value
		 *\~french
		 *\brief		Définit le squelette
		 *\param[in]	skeleton	La nouvelle valeur
		 */
		C3D_API void setSkeleton( SkeletonSPtr skeleton );
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
		C3D_API MeshAnimation & createAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Removes an animation
		 *\param[in]	name	The animation name
		 *\~french
		 *\brief		Retire une animation
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void removeAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur sur le début des sous maillages
		 */
		inline SubmeshPtrArrayIt begin()
		{
			return m_submeshes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the begin of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur constant sur le début des sous maillages
		 */
		inline SubmeshPtrArrayConstIt begin()const
		{
			return m_submeshes.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur sur la fin des sous maillages
		 */
		inline SubmeshPtrArrayIt end()
		{
			return m_submeshes.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the end of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin des sous maillages
		 */
		inline SubmeshPtrArrayConstIt end()const
		{
			return m_submeshes.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the modify status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modification
		 *\return		La valeur
		 */
		inline bool isModified()const
		{
			return m_modified;
		}
		/**
		 *\~english
		 *\brief		Retrieves the submesh count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de sous maillages
		 *\return		La valeur
		 */
		inline uint32_t getSubmeshCount()const
		{
			return uint32_t( m_submeshes.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the collision box
		 *\return		The value
		 *\~french
		 *\brief		Récupère la boîte de collision
		 *\return		La valeur
		 */
		inline castor::BoundingBox const & getBoundingBox()const
		{
			return m_box;
		}
		/**
		 *\~english
		 *\brief		Retrieves the collision sphere
		 *\return		The value
		 *\~french
		 *\brief		Récupère la sphère de collision
		 *\return		La valeur
		 */
		inline castor::BoundingSphere const & getBoundingSphere()const
		{
			return m_sphere;
		}
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The value
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		La valeur
		 */
		inline SkeletonSPtr getSkeleton()const
		{
			return m_skeleton;
		}
		/**
		 *\~english
		 *\return		The serialisable status.
		 *\~french
		 *\return		Le statut de sérialisation.
		 */
		inline bool isSerialisable()const
		{
			return m_serialisable;
		}
		/**
		 *\~english
		 *\brief		Sets the serialisable status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut de sérialisation.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSerialisable( bool value )
		{
			m_serialisable = value;
		}

		Scene * getScene()const
		{
			return m_scene;
		}

	private:
		friend class MeshGenerator;
		CU_DeclareVector( AnimationPtrStrMap, AnimationMap );

		Scene * m_scene;
		bool m_modified;
		castor::BoundingBox m_box;
		castor::BoundingSphere m_sphere;
		SubmeshPtrArray m_submeshes;
		SkeletonSPtr m_skeleton;
		bool m_serialisable{ true };

		friend class BinaryWriter< Mesh >;
		friend class BinaryParser< Mesh >;
	};
}

#endif
