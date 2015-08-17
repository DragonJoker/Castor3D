﻿/*
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
#ifndef ___C3D_SUBMESH_H___
#define ___C3D_SUBMESH_H___

#include "Castor3DPrerequisites.hpp"
#include "Mesh.hpp"
#include "FaceIndices.hpp"
#include "FaceInfos.hpp"
#include "VertexBoneData.hpp"
#include "VertexGroup.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The submesh representation
	\remark		A submesh holds its buffers (vertex, normas and texture) its smoothgroups and its combobox
	\~french
	\brief		Representation d'un submesh
	\remark		Un submesh est sous partie d'un mesh. Il possede ses propres tampons (vertex, normales et texture coords) ses smoothgroups et ses combobox
	*/
	class C3D_API Submesh
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Submesh text loader
		\~french
		\brief		Loader texte de Submesh
		*/
		class C3D_API TextLoader
			: public Castor::Loader< Submesh, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
			, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Function operator
			 *\param[in]	p_submesh	The submesh to write in the file
			 *\param[in]	p_file		The file in which submesh is written
			 *\return		\p true if OK
			 *\~french
			 *\brief		Opérateur fonction
			 *\param[in]	p_submesh	Le submesh à écrire dans le fichier
			 *\param[in]	p_file		Le fichier dans lequel le submesh est écrit
			 *\return		\p true si tout s'est bien passé
			 */
			virtual bool operator()( Submesh const & p_submesh, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		MovableObject loader
		\~english
		\brief		Loader de MovableObject
		*/
		class C3D_API BinaryParser
			: public Castor3D::BinaryParser< Submesh >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Fill( Submesh const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( Submesh & p_obj, BinaryChunk & p_chunk )const;
		};

	private:
		DECLARE_LIST( Castor::ByteArray, BytePtr );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pMesh		The parent mesh
		 *\param[in]	p_pEngine	The core engine
		 *\param[in]	p_uiID		The submesh ID
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pMesh		Le mesh parent
		 *\param[in]	p_pEngine	Le moteur
		 *\param[in]	p_uiID		L'ID du submesh
		 */
		Submesh( MeshRPtr p_pMesh, Engine * p_pEngine = nullptr, uint32_t p_uiID = 1 );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Submesh();
		/**
		 *\~english
		 *\brief		Initialises the submesh
		 *\~french
		 *\brief		Initialise le submesh
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Cleans the submesh
		 *\~french
		 *\brief		Nettoie le submesh
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Computes the containers (cube and sphere)
		 *\~french
		 *\brief		Calcule les conteneurs (cube et sphère)
		 */
		void ComputeContainers();
		/**
		 *\~english
		 *\return		The faces number
		 *\~french
		 *\return		Le nombre de faces de ce submesh
		 */
		uint32_t GetFaceCount()const;
		/**
		 *\~english
		 *\return		The points count
		 *\~french
		 *\return		Le nombre de vertices de ce submesh
		 */
		uint32_t GetPointsCount()const;
		/**
		 *\~english
		 *\brief		Tests if the given Point3r is in mine
		 *\param[in]	p_vertex	The vertex to test
		 *\param[in]	p_precision	The comparison precision
		 *\return		The index of the vertex equal to parameter, -1 if not found
		 *\~french
		 *\brief		Teste si le point donné fait partie de ceux de ce submesh
		 *\param[in]	p_vertex	Le point à tester
		 *\param[in]	p_precision	La précision de comparaison
		 *\return		L'index du point s'il a été trouvé, -1 sinon
		 */
		int IsInMyPoints( Castor::Point3r const & p_vertex, double p_precision );
		/**
		 *\~english
		 *\brief		Creates and adds a vertex to my list
		 *\param[in]	x	The vertex X coordinate
		 *\param[in]	y	The vertex Y coordinate
		 *\param[in]	z	The vertex Z coordinate
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée un Vertex à partir des coordonnées données et l'ajoute à la liste
		 *\param[in]	x	Coordonnée X
		 *\param[in]	y	Coordonnée Y
		 *\param[in]	z	Coordonnée Y
		 *\return		Le vertex créé
		 */
		BufferElementGroupSPtr AddPoint( real x, real y, real z );
		/**
		 *\~english
		 *\brief		Adds a vertex to my list
		 *\param[in]	p_v	The vertex to add
		 *\return		The vertex
		 *\~french
		 *\brief		Crée un Vertex à partir du point donné et l'ajoute à la liste
		 *\param[in]	p_v	Le point
		 *\return		Le vertex créé
		 */
		BufferElementGroupSPtr AddPoint( Castor::Point3r const & p_v );
		/**
		 *\~english
		 *\brief		Creates and adds a vertex to my list
		 *\param[in]	p_v	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée un Vertex à partir des coordonnées données et l'ajoute à la liste
		 *\param[in]	p_v	Les coordonnées du point
		 *\return		Le vertex créé
		 */
		BufferElementGroupSPtr AddPoint( real * p_v );
		/**
		 *\~english
		 *\brief		Adds a points list to my list
		 *\param[in]	p_vertices	The vertices
		 *\~french
		 *\brief		Ajoute des points à la liste
		 *\param[in]	p_vertices	Les vertices
		 */
		void AddPoints( stVERTEX_GROUP const & p_vertices );
		/**
		 *\~english
		 *\brief		Clears this submesh's face array
		 *\~french
		 *\brief		Vide le tableau de faces
		 */
		void ClearFaces();
		/**
		 *\~english
		 *\brief		Creates and adds a face to the submesh
		 *\param[in]	a			The first face's vertex index
		 *\param[in]	b			The second face's vertex index
		 *\param[in]	c			The third face's vertex index
		 *\return		The created face
		 *\~french
		 *\brief		Crée et ajoute une face au submesh
		 *\param[in]	a			L'index du premier vertex
		 *\param[in]	b			L'index du second vertex
		 *\param[in]	c			L'index du troisième vertex
		 *\return		La face créée
		 */
		FaceSPtr AddFace( uint32_t a, uint32_t b, uint32_t c );
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh
		 *\param[in]	p_arrayFaces	The faces
		 *\param[in]	p_uiNbFaces		The faces count
		 *\~french
		 *\brief		Crée et ajoute une face au submesh
		 *\param[in]	p_arrayFaces	Les faces
		 *\param[in]	p_uiNbFaces		Le nombre de faces
		 */
		void AddFaceGroup( stFACE_INDICES * p_pFaces, uint32_t p_uiNbFaces );
		/**
		 *\~english
		 *\brief		Creates and adds a quad face to the submesh
		 *\param[in]	a			The first face's vertex index
		 *\param[in]	b			The second face's vertex index
		 *\param[in]	c			The third face's vertex index
		 *\param[in]	d			The fourth face's vertex index
		 *\param[in]	p_ptMinUV	The UV of the bottom left corner
		 *\param[in]	p_ptMaxUV	The UV of the top right corner
		 *\return		The created face
		 *\~french
		 *\brief		Crée et ajoute une face à 4 côtés au submesh
		 *\param[in]	a			L'index du premier vertex
		 *\param[in]	b			L'index du second vertex
		 *\param[in]	c			L'index du troisième vertex
		 *\param[in]	d			L'index du quatrième vertex
		 *\param[in]	p_ptMinUV	L'UV du coin bas gauche
		 *\param[in]	p_ptMaxUV	L'UV du coin haut droit
		 *\return		La face créée
		 */
		void AddQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d, Castor::Point3r const & p_ptMinUV = Castor::Point3r(), Castor::Point3r const & p_ptMaxUV = Castor::Point3r( 1, 1, 1 ) );
		/**
		 *\~english
		 *\brief		Generates the 3D buffers
		 *\~french
		 *\brief		Genère les buffers 3D
		 */
		void GenerateBuffers();
		/**
		 *\~english
		 *\brief		Clones the submesh and returns the clone
		 *\return	The clone
		 *\~french
		 *\brief		Clône le submesh
		 *\return	Le clône
		 */
		SubmeshSPtr Clone();
		/**
		 *\~english
		 *\brief		Recreates the Vertex and Index buffers
		 *\~french
		 *\brief		Recrée les Vertex et Index buffers
		 */
		void ResetGpuBuffers();
		/**
		 *\~english
		 *\brief		Draws the submesh
		 *\param[in]	p_eMode	The render mode
		 *\param[in]	p_pass	The Pass containing material informations
		 *\~french
		 *\brief		Dessine le submesh
		 *\param[in]	p_eMode	Le mode de rendu
		 *\param[in]	p_pass	La Pass contenant les informations de matériau
		 */
		void Draw( eTOPOLOGY p_eMode, Pass const & p_pass );
		/**
		 *\~english
		 *\brief		Creates faces from the points
		 *\remark		This function assumes the points are sorted like triangles fan
		 *\~french
		 *\brief		Crée les faces à partir des points
		 *\remark		Cette fonction suppose que les points sont tirés à la manière triangles fan
		 */
		void ComputeFacesFromPolygonVertex();
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		void ComputeNormals( bool p_bReverted = false );
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face
		 *\param[in]	p_pFace	The face
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée
		 *\param[in]	p_pFace	La face
		 */
		void ComputeNormals( FaceSPtr p_pFace );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the given face
		 *\param[in]	p_pFace	The face
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée
		 *\param[in]	p_pFace	La face
		 */
		void ComputeTangents( FaceSPtr p_pFace );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh
		 *\remark		This function supposes the normals are defined
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du submesh
		 *\remark		Cette fonction suppose que les normales sont définies
		 */
		void ComputeTangentsFromNormals();
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh
		 *\remark		This function supposes bitangents and normals are defined
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du submesh
		 *\remark		Cette fonction suppose que les bitangentes et les normales sont définies
		 */
		void ComputeTangentsFromBitangents();
		/**
		 *\~english
		 *\brief		Computes bitangent for each vertex of the submesh
		 *\remark		This function supposes the tangents and normals are defined
		 *\~french
		 *\brief		Calcule la bitangente pour chaque vertex du submesh
		 *\remark		Cette fonction suppose que les tangentes et les normales sont définies
		 */
		void ComputeBitangents();
		/**
		 *\~english
		 *\brief		Sorts the face from farthest to nearest from the camera
		 *\param[in]	p_ptCamera	The camera position, relative to submesh
		 *\~french
		 *\brief		Trie les faces des plus éloignées aux plus proches de la caméra
		 *\param[in]	p_ptCamera	La position de la caméra, relative au submesh
		 */
		void SortFaces( Castor::Point3r const & p_ptCamera );
		/**
		 *\~english
		 *\brief		Increments instance count
		 *\param[in]	p_material	The material for which the instance count is incremented
		 *\~french
		 *\brief		Incrémente le compte d'instances
		 *\param[in]	p_material	Le matériau pour lequel le compte est incrémenté
		 */
		void Ref( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Decrements instance count
		 *\param[in]	p_material	The material for which the instance count is decremented
		 *\~french
		 *\brief		Décrémente le compte d'instances
		 *\param[in]	p_material	Le matériau pour lequel le compte est décrémenté
		 */
		void UnRef( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Retrieves the instances count
		 *\param[in]	p_material	The material for which the instance count is retrieved
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'instances
		 *\param[in]	p_material	Le matériau pour lequel le compte est récupéré
		 *\return		La valeur
		 */
		uint32_t GetRefCount( MaterialSPtr p_material )const;
		/**
		 *\~english
		 *\brief		Retrieves an iterator over the first vertex of the submesh
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier vertex du submesh
		 *\return		L'itérateur
		 */
		VertexPtrArrayIt VerticesBegin()
		{
			return m_points.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator over the first vertex of the submesh
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier vertex du submesh
		 *\return		L'itérateur
		 */
		VertexPtrArrayConstIt VerticesBegin()const
		{
			return m_points.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator over the end of the vertex array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de vertex
		 *\return		L'itérateur
		 */
		VertexPtrArrayIt VerticesEnd()
		{
			return m_points.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator over the end of the vertex array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin du tableau de vertex
		 *\return		L'itérateur
		 */
		VertexPtrArrayConstIt VerticesEnd()const
		{
			return m_points.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator over the first vertex of the submesh
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier vertex du submesh
		 *\return		L'itérateur
		 */
		FacePtrArrayIt FacesBegin()
		{
			return m_arrayFaces.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator over the first vertex of the submesh
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier vertex du submesh
		 *\return		L'itérateur
		 */
		FacePtrArrayConstIt FacesBegin()const
		{
			return m_arrayFaces.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator over the end of the vertex array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de vertex
		 *\return		L'itérateur
		 */
		FacePtrArrayIt FacesEnd()
		{
			return m_arrayFaces.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator over the end of the vertex array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin du tableau de vertex
		 *\return		L'itérateur
		 */
		FacePtrArrayConstIt FacesEnd()const
		{
			return m_arrayFaces.end();
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
			return m_pParentMesh->GetSkeleton();
		}
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le material
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetDefaultMaterial( MaterialSPtr p_mat )
		{
			m_defaultMaterial = p_mat;
		}
		/**
		 *\~english
		 *\brief		Retrieves the point at given index
		 *\param[in]	p_uiIndex	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le point à l'index donné
		 *\param[in]	p_uiIndex	L'index
		 *\return		La valeur
		 */
		inline BufferElementGroupSPtr operator []( uint32_t p_uiIndex )const
		{
			CASTOR_ASSERT( p_uiIndex < m_points.size() );
			return m_points[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Retrieves the point at given index
		 *\param[in]	p_uiIndex	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le point à l'index donné
		 *\param[in]	p_uiIndex	L'index
		 *\return		La valeur
		 */
		inline BufferElementGroupSPtr GetPoint( uint32_t p_uiIndex )const
		{
			CASTOR_ASSERT( p_uiIndex < m_points.size() );
			return m_points[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le material
		 *\return		La valeur
		 */
		inline MaterialSPtr GetDefaultMaterial()const
		{
			return m_defaultMaterial.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the cube bounding box
		 *\return		The value
		 *\~french
		 *\brief		Récupère la boundig box cube
		 *\return		La valeur
		 */
		inline Castor::CubeBox const & GetCubeBox()const
		{
			return m_box;
		}
		/**
		 *\~english
		 *\brief		Retrieves the cube bounding box
		 *\return		The value
		 *\~french
		 *\brief		Récupère la boundig box cube
		 *\return		La valeur
		 */
		inline Castor::CubeBox & GetCubeBox()
		{
			return m_box;
		}
		/**
		 *\~english
		 *\brief		Retrieves the sphere bounding box
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bounding box sphere
		 *\return		La valeur
		 */
		inline Castor::SphereBox const & GetSphere()const
		{
			return m_sphere;
		}
		/**
		 *\~english
		 *\brief		Retrieves the sphere bounding box
		 *\return		The value
		 *\~french
		 *\brief		Récupère la bounding box sphere
		 *\return		La valeur
		 */
		inline Castor::SphereBox & GetSphere()
		{
			return m_sphere;
		}
		/**
		 *\~english
		 *\brief		Retrieves the points array
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tableau de points
		 *\return		La valeur
		 */
		inline VertexPtrArray const & GetPoints()const
		{
			return m_points;
		}
		/**
		 *\~english
		 *\brief		Retrieves the points array
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tableau de points
		 *\return		La valeur
		 */
		inline VertexPtrArray & GetPoints()
		{
			return m_points;
		}
		/**
		 *\~english
		 *\brief		Retrieves the face at given index
		 *\param[in]	p_uiIndex	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère récupère la face à l'index donné
		 *\param[in]	p_uiIndex	L'index
		 *\return		La valeur
		 */
		inline FaceSPtr GetFace( uint32_t p_uiIndex )const
		{
			CASTOR_ASSERT( p_uiIndex < m_arrayFaces.size() );
			return m_arrayFaces[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Retrieves the faces array
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tableau de faces
		 *\return		La valeur
		 */
		inline FacePtrArray const & GetFaces()const
		{
			return m_arrayFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves the faces array
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tableau de faces
		 *\return		La valeur
		 */
		inline FacePtrArray & GetFaces()
		{
			return m_arrayFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves the BufferDeclaration
		 *\return		The BufferDeclaration
		 *\~french
		 *\brief		Récupère la BufferDeclaration
		 *\return		La BufferDeclaration
		 */
		inline BufferDeclarationSPtr GetDeclaration()
		{
			return m_pDeclaration;
		}
		/**
		 *\~english
		 *\brief		Retrieves the IndexBuffer of wanted primitive type
		 *\return		The IndexBuffer
		 *\~french
		 *\brief		Récupère l'IndexBuffer pour le type de primitive voulu
		 *\return		Le IndexBuffer
		 */
		inline GeometryBuffersSPtr GetGeometryBuffers()const
		{
			return m_pGeometryBuffers;
		}
		/**
		 *\~english
		 *\brief		Retrieves the initialisation status
		 *\return		\p true if initialised
		 *\~french
		 *\brief		Récupère le statut d'initialisation
		 *\return		\p true si initialisé
		 */
		inline bool	IsInitialised()const
		{
			return m_bInitialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent mesh
		 *\return		The value
		 *\~french
		 *\brief		Récupère le mesh parent
		 *\return		La valeur
		 */
		inline MeshRPtr GetParent()const
		{
			return m_pParentMesh;
		}
		/**
		 *\~english
		 *\brief		Retrieves the shader program flags
		 *\return		The value
		 *\~french
		 *\brief		Récupère les indicateurs de shader
		 *\return		La valeur
		 */
		inline uint32_t GetProgramFlags()const
		{
			return m_uiProgramFlags;
		}
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh
		 *\param[in]	p_arrayFaces	The faces
		 *\~french
		 *\brief		Crée et ajoute une face au submesh
		 *\param[in]	p_arrayFaces	Les faces
		 */
		template< uint32_t Count > void AddFaceGroup( stFACE_INDICES( & p_pFaces )[Count] )
		{
			AddFaceGroup( p_pFaces, Count );
		}
		/**
		 *\~english
		 *\brief		Retrieves the Engine
		 *\~french
		 *\brief		Récupère l'Engine
		 */
		virtual Engine * GetEngine()const
		{
			return m_pEngine;
		}

	private:
		FaceSPtr DoAddFace( const FaceSPtr p_face );
		void DoGenerateVertexBuffer();
		void DoGenerateIndexBuffer();
		void DoGenerateMatrixBuffer();
		void DoUpdateDeclaration();
		void DoCleanupGeometryBuffers();
		void DoCreateGeometryBuffers();
		bool DoPrepareGeometryBuffers( Pass const & p_pass );

	private:
		//!\~english The core engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english The submesh ID	\~french L'id du sbmesh
		uint32_t m_uiID;
		//!\~english The submesh instances count	\~french Le nombre d'instances du submesh
		std::map< MaterialSPtr, uint32_t > m_uiInstanceCount;
		//!\~english The faces in the submesh	\~french Le tableau de faces
		FacePtrArray m_arrayFaces;
		//!\~english Tells if normals exist or need to be computed	\~french Dit si les normales existent ou doivent être calculées
		bool m_bHasNormals;
		//!\~english The Material assigned at creation	\~french Le Materiau affecté à la création
		MaterialWPtr m_defaultMaterial;
		//!\~english The combo box container	\~french Le conteneur boîte
		Castor::CubeBox m_box;
		//!\~english The spheric container	\~french Le conteneur sphère
		Castor::SphereBox m_sphere;
		//!\~english The vertex data array	\~french Le tableau de données des sommets
		BytePtrList m_pointsData;
		//!\~english The vertex pointer array	\~french Le tableau de sommets
		VertexPtrArray m_points;
		//!\~english Vertex elements declaration	\~french Déclaration des éléments d'un sommet
		BufferDeclarationSPtr m_pDeclaration;
		//!\~english The transformed camera position at last sort	\~french La position transformée de la caméra au dernier tri
		Castor::Point3r m_ptCameraPosition;
		//!\~english The parent mesh	\~french Le mesh parent
		MeshRPtr m_pParentMesh;
		//!\~english The shader program flags	\~french Les indicateurs pour le shader
		uint32_t m_uiProgramFlags;
		//!\~english Pointer over geometry buffers	\~french Pointeur sur les buffers de la géométrie
		GeometryBuffersSPtr	m_pGeometryBuffers;
		//!\~english Tells the renderer has been initialised	\~french Dit que le renderer a été initialisé
		bool m_bInitialised;
		//!\~english The actual draw type	\~french Le type de dessin actuel
		eTOPOLOGY m_eCurDrawType;
		//!\~english The previous call draw type	\~french Le type de dessin lors du précédent appel
		eTOPOLOGY m_ePrvDrawType;
		//!\~english Tells the VAO needs reininitialisation	\~french Dit que le VAO a besoin d'être réinitialisé
		bool m_bDirty;
	};
}

#pragma warning( pop )

#endif
