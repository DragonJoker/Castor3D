/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_MESH_H___
#define ___C3D_MESH_H___

#include "Castor3DPrerequisites.hpp"
#include "MeshGenerator.hpp"

#include "Animation/Animable.hpp"
#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include <CubeBox.hpp>
#include <SphereBox.hpp>
#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The mesh representation
	\remark		A mesh is a collection of submeshes.
	\~french
	\brief		Représentation d'un maillage
	\remark		Un maillage est une collectionde sous maillages.
	*/
	class Mesh
		: public Castor::Resource< Mesh >
		, public Animable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name	This mesh name.
		 *\param[in]	p_scene	The parent scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name	Le nom du maillage.
		 *\param[in]	p_scene	La scèene parente.
		 */
		C3D_API Mesh( Castor::String const & p_name, Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Mesh();
		/**
		 *\~english
		 *\brief		Clears all submeshes
		 *\~french
		 *\brief		Supprime les sous maillages
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Computes the collision box and sphere.
		 *\~french
		 *\brief		Calcule la boîte et la sphère de collision.
		 */
		C3D_API void ComputeContainers();
		/**
		 *\~english
		 *\brief		Returns the total faces count
		 *\return		The faces count
		 *\~french
		 *\brief		Récupère le nombre total de faces
		 *\return		Le compte
		 */
		C3D_API uint32_t GetFaceCount()const;
		/**
		 *\~english
		 *\brief		Returns the total vertex count
		 *\return		The vertex count
		 *\~french
		 *\brief		Récupère le nombre total de sommets
		 *\return		Le compte
		 */
		C3D_API uint32_t GetVertexCount()const;
		/**
		 *\~english
		 *\brief		Tries to retrieve the submesh at the given index
		 *\param[in]	p_index	The wanted submesh index
		 *\return		The found submesh, nullptr if not found
		 *\~french
		 *\brief		Essaie de récupérer le sous maillage à l'indice donné
		 *\param[in]	p_index	L'indice du sous maillage voulu
		 *\return		Le sous maillage trouvé, nullptr si non trouvé
		 */
		C3D_API SubmeshSPtr GetSubmesh( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Creates a submesh
		 *\return		The created submesh
		 *\~french
		 *\brief		Crée un sous maillage
		 *\return		Le sous maillage créé
		 */
		C3D_API SubmeshSPtr CreateSubmesh();
		/**
		 *\~english
		 *\brief		Deletes a given submesh if it is in the mesh's submeshes
		 *\param[in]	p_pSubmesh	The submesh to delete
		 *\~french
		 *\brief		Supprime le submesh s'il est dans les submesh du mesh
		 *\param[in]	p_pSubmesh	Le submesh à supprimer
		 */
		C3D_API void DeleteSubmesh( SubmeshSPtr & p_pSubmesh );
		/**
		 *\~english
		 *\brief		Clones the mesh, with a new name
		 *\param[in]	p_name	The name of the cloned mesh
		 *\return		The cloned mesh
		 *\~french
		 *\brief		Clone le maillage, avec un nouveau nom
		 *\param[in]	p_name	Le nom du clone
		 *\return		Le clone
		 */
		C3D_API MeshSPtr Clone( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		C3D_API void ComputeNormals( bool p_reverted = false );
		/**
		*\~english
		*\brief		Increments submeshes' instance count
		*\param[in]	p_material	The material for which the instance count is incremented
		*\~french
		*\brief		Incrémente le compte d'instances des sous maillages
		*\param[in]	p_material	Le matériau pour lequel le compte est incrémenté
		*/
		C3D_API void Ref( MaterialSPtr p_material );
		/**
		*\~english
		*\brief		Decrements submeshes' instance count
		*\param[in]	p_material	The material for which the instance count is decremented
		*\~french
		*\brief		Décrémente le compte d'instances des sous maillages
		*\param[in]	p_material	Le matériau pour lequel le compte est décrémenté
		*/
		C3D_API void UnRef( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Sets the skeleton
		 *\param[in]	p_skeleton	The new value
		 *\~french
		 *\brief		Définit le squelette
		 *\param[in]	p_skeleton	La nouvelle valeur
		 */
		C3D_API void SetSkeleton( SkeletonSPtr p_skeleton );
		/**
		 *\~english
		 *\brief		Creates an animation
		 *\param[in]	p_name	The animation name
		 *\return		The animation
		 *\~french
		 *\brief		Crée une animation
		 *\param[in]	p_name	Le nom de l'animation
		 *\return		l'animation
		 */
		C3D_API MeshAnimationSPtr CreateAnimation( Castor::String const & p_name );
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
		inline bool IsModified()const
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
		inline uint32_t GetSubmeshCount()const
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
		inline Castor::CubeBox const & GetCollisionBox()const
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
		inline Castor::SphereBox const & GetCollisionSphere()const
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
		inline SkeletonSPtr GetSkeleton()const
		{
			return m_skeleton;
		}

	protected:
		friend class MeshGenerator;
		DECLARE_VECTOR( AnimationPtrStrMap, AnimationMap );
		//!\~english	Tells whether or not the mesh is modified.
		//!\~french		Dit si le maillage est modifié.
		bool m_modified;
		//!\~english	The collision box.
		//!\~french		La boîte de collision.
		Castor::CubeBox m_box;
		//!\~english	The collision sphere.
		//!\~french		La sphere de collision.
		Castor::SphereBox m_sphere;
		//!\~english	The submeshes array.
		//!\~french		Le tableau de sous maillages.
		SubmeshPtrArray m_submeshes;
		//!\~english	The skeleton.
		//!\~french		Le squelette.
		SkeletonSPtr m_skeleton;

		friend class BinaryWriter< Mesh >;
		friend class BinaryParser< Mesh >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for Mesh.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour Mesh.
	*/
	template<>
	struct ChunkTyper< Mesh >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_MESH;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		MovableObject loader
	\~english
	\brief		Loader de MovableObject
	*/
	template<>
	class BinaryWriter< Mesh >
		: public BinaryWriterBase< Mesh >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	p_obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	p_obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool DoWrite( Mesh const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		MovableObject loader
	\~english
	\brief		Loader de MovableObject
	*/
	template<>
	class BinaryParser< Mesh >
		: public BinaryParserBase< Mesh >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_chunk	The chunk containing data
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_chunk	Le chunk contenant les données
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool DoParse( Mesh & p_obj )override;
	};
}

#endif
