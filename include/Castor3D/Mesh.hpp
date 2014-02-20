/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_Mesh___
#define ___C3D_Mesh___

#include "Prerequisites.hpp"
#include "Animable.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\~english
	\brief		Mesh category is used to create a mesh's vertices, it is created by MeshFactory
	\remark		Custom mesh category doesn't generate vertices.
	\~french
	\brief		La catégorie de maillage est utilisée afin de créer les sommets du maillage, c'est créé via la MeshFactory
	\remark		La categorie de type Custom ne génère pas de points
	*/
	class MeshCategory
	{
	private:
		//!\~english	The mesh type	\~french	Le type de mesh
		eMESH_TYPE m_eMeshType;
		//!\~english	The mesh	\~french	Le mesh
		Mesh * m_pMesh;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eMeshType	The mesh type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eMeshType	Le type de maillage
		 */
		MeshCategory( eMESH_TYPE p_eMeshType=eMESH_TYPE_CUSTOM );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MeshCategory();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		Cylinder
		 *\~french
		 *\brief		Fonction de création, utilisée par la Factory
		 *\return		Un cylindre
		 */
		static MeshCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Generates the mesh (vertices and faces)
		 *\~french
		 *\brief		Génère le maillage
		 */
		virtual void Generate();
		/**
		 *\~english
		 *\brief		Initialises the mesh
		 *\remark		Calls Generate
		 *\param[in]	p_arrayFaces		The faces counts
		 *\param[in]	p_arrayDimensions	The mesh dimensions
		 *\~french
		 *\brief		Initialise le maillage
		 *\remark		Appelle Generate
		 *\param[in]	p_arrayFaces		Les nombres de faces
		 *\param[in]	p_arrayDimensions	Les dimensions du maillage
		 */
		virtual void Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		virtual void ComputeNormals( bool p_bReverted=false );
		/**
		 *\~english
		 *\brief		Retrieves the mesh type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de maillage
		 *\return		La valeur
		 */
		inline eMESH_TYPE GetMeshType()const { return m_eMeshType; }
		/**
		 *\~english
		 *\brief		Retrieves the mesh
		 *\return		The value
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		La valeur
		 */
		inline Mesh * GetMesh()const { return m_pMesh; }
		/**
		 *\~english
		 *\brief		Sets the mesh
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le maillage
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetMesh	( Mesh * val ) { m_pMesh = val; }
	};
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
	class C3D_API Mesh : CuNonCopyable, public Castor::Resource< Mesh >, public Animable
	{
	protected:
		friend class MeshCategory;
		DECLARE_VECTOR( AnimationPtrStrMap, AnimationMap );
		//!\~english The mesh category	\~french La catégorie de maillage
		MeshCategorySPtr m_pMeshCategory;
		//!\~english Tells whether or not the mesh is modified	\~french Dit si le maillage est modifié
		bool m_modified;
		//!\~english The collision box	\~french La boîte de collision
		Castor::CubeBox m_box;
		//!\~english The collision sphere	\~french La sphere de collision
		Castor::SphereBox m_sphere;
		//!\~english The submeshes array	\~french Le tableau de sous maillages
		SubmeshPtrArray m_submeshes;
		//!\~english	The MeshCategory factory	\~french	la fabrique de MeshCategory
		MeshFactory & m_factory;
		//!\~english	The parent engine	\~french	Le moteur parent
		Engine * m_pEngine;
		//!\~english	The skeleton	\~french	Le squelette
		SkeletonSPtr m_pSkeleton;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine	The parent engine
		 *\param[in]	p_factory	The factory
		 *\param[in]	p_eMeshType	The mesh type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine	Le moteur parent
		 *\param[in]	p_factory	La fabrique
		 *\param[in]	p_eMeshType	Le type de maillage
		 */
		Mesh( Engine * p_pEngine, MeshFactory & p_factory, eMESH_TYPE p_eMeshType = eMESH_TYPE_CUSTOM );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine	The parent engine
		 *\param[in]	p_factory	The factory
		 *\param[in]	p_name		This mesh name
		 *\param[in]	p_eMeshType	The mesh type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine	Le moteur parent
		 *\param[in]	p_factory	La fabrique
		 *\param[in]	p_name		Le nom du maillage
		 *\param[in]	p_eMeshType	Le type de maillage
		 */
		Mesh( Engine * p_pEngine, MeshFactory & p_factory, Castor::String const & p_name, eMESH_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Mesh();
		/**
		 *\~english
		 *\brief		Registers all meshes types into factory
		 *\~french
		 *\brief		Enregistre tous les types de maillages dans la fabrique
		 */
		static void Register( MeshFactory & p_factory );
		/**
		 *\~english
		 *\brief		Clears all submeshes
		 *\~french
		 *\brief		Supprime les sous maillages
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Initialises mesh's vertex and faces
		 *\param[in]	p_arrayFaces		The faces counts
		 *\param[in]	p_arrayDimensions	The mesh dimensions
		 *\~french
		 *\brief		Initialise les sommets et faces du maillage
		 *\param[in]	p_arrayFaces		Les nombres de faces
		 *\param[in]	p_arrayDimensions	Les dimensions du maillage
		 */
		void Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions );
		/**
		 *\~english
		 *\brief		Computes the collision box and sphere.
		 *\~french
		 *\brief		Calcule la boîte et la sphère de collision.
		 */
		void ComputeContainers();
		/**
		 *\~english
		 *\brief		Returns the total faces count
		 *\return		The faces count
		 *\~french
		 *\brief		Récupère le nombre total de faces
		 *\return		Le compte
		 */
		uint32_t GetNbFaces()const;
		/**
		 *\~english
		 *\brief		Returns the total vertex count
		 *\return		The vertex count
		 *\~french
		 *\brief		Récupère le nombre total de sommets
		 *\return		Le compte
		 */
		uint32_t GetNbVertex()const;
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
		SubmeshSPtr GetSubmesh( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Creates a submesh
		 *\return		The created submesh
		 *\~french
		 *\brief		Crée un sous maillage
		 *\return		Le sous maillage créé
		 */
		SubmeshSPtr CreateSubmesh();
		/**
		 *\~english
		 *\brief		Deletes a given submesh if it is in the mesh's submeshes
		 *\param[in]	p_pSubmesh	The submesh to delete
		 *\~french
		 *\brief		Supprime le submesh s'il est dans les submesh du mesh
		 *\param[in]	p_pSubmesh	Le submesh à supprimer
		 */
		void DeleteSubmesh( SubmeshSPtr & p_pSubmesh );
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
		MeshSPtr Clone( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		void ComputeNormals( bool p_bReverted=false );
		/**
		 *\~english
		 *\brief		Initialises vertex buffers
		 *\~french
		 *\brief		Initialise les tampons de sommets
		 */
		void GenerateBuffers();
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur sur le début des sous maillages
		 */
		SubmeshPtrArrayIt Begin() { return m_submeshes.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the begin of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur constant sur le début des sous maillages
		 */
		SubmeshPtrArrayConstIt Begin()const { return m_submeshes.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur sur la fin des sous maillages
		 */
		SubmeshPtrArrayIt End() { return m_submeshes.end(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the end of the submeshes
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin des sous maillages
		 */
		SubmeshPtrArrayConstIt End()const { return m_submeshes.end(); }
		/**
		 *\~english
		 *\brief		Retrieves the modify status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modification
		 *\return		La valeur
		 */
		inline bool IsModified()const { return m_modified;}
		/**
		 *\~english
		 *\brief		Retrieves the submesh count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de sous maillages
		 *\return		La valeur
		 */
		inline uint32_t GetSubmeshCount()const { return uint32_t( m_submeshes.size() ); }
		/**
		 *\~english
		 *\brief		Retrieves the mesh category type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de la catégorie de maillage
		 *\return		La valeur
		 */
		inline eMESH_TYPE GetMeshType()const { return m_pMeshCategory->GetMeshType();}
		/**
		 *\~english
		 *\brief		Retrieves the collision box
		 *\return		The value
		 *\~french
		 *\brief		Récupère la boîte de collision
		 *\return		La valeur
		 */
		inline Castor::CubeBox const & GetCollisionBox()const { return m_box; }
		/**
		 *\~english
		 *\brief		Retrieves the collision sphere
		 *\return		The value
		 *\~french
		 *\brief		Récupère la sphère de collision
		 *\return		La valeur
		 */
		inline Castor::SphereBox const & GetCollisionSphere()const { return m_sphere; }
		/**
		 *\~english
		 *\brief		Retrieves the skeleton
		 *\return		The value
		 *\~french
		 *\brief		Récupère le squelette
		 *\return		La valeur
		 */
		inline SkeletonSPtr GetSkeleton()const { return m_pSkeleton; }
		/**
		 *\~english
		 *\brief		Sets the skeleton
		 *\param[in]	p_pSkeleton	The new value
		 *\~french
		 *\brief		Définit le squelette
		 *\param[in]	p_pSkeleton	La nouvelle valeur
		 */
		inline void SetSkeleton( SkeletonSPtr p_pSkeleton ) { m_pSkeleton = p_pSkeleton; }
		/**
		 *\~english
		 *\brief		Increments submeshes' instance count
		 *\~french
		 *\brief		Incrémente le compte d'instances des sous maillages
		 */
		void Ref();
		/**
		 *\~english
		 *\brief		Decrements submeshes' instance count
		 *\~french
		 *\brief		Décrémente le compte d'instances des sous maillages
		 */
		void UnRef();
	};
}

#pragma warning( pop )

#endif
