/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_SUBMESH_H___
#define ___C3D_SUBMESH_H___

#include "Castor3DPrerequisites.hpp"
#include "Mesh.hpp"
#include "FaceIndices.hpp"
#include "FaceInfos.hpp"
#include "Face.hpp"
#include "VertexGroup.hpp"

#include "Mesh/Skeleton/VertexBoneData.hpp"
#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The submesh representation.
	\remark		A submesh holds its buffers (vertex, normals and texture) and its combobox.
	\~french
	\brief		Representation d'un sous-maillage.
	\remark		Un sous-maillage est sous partie d'un maillage. Il possede ses propres tampons (vertex, normales et texture coords) et ses combobox.
	*/
	class Submesh
		: public Castor::OwnedBy< Scene >
		, public std::enable_shared_from_this< Submesh >
	{
	private:
		DECLARE_LIST( Castor::ByteArray, BytePtr );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_mesh	The parent mesh.
		 *\param[in]	p_scene	The scene.
		 *\param[in]	p_id	The submesh ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_mesh	Le mesh parent.
		 *\param[in]	p_scene	La scène.
		 *\param[in]	p_id	L'ID du sous-maillage.
		 */
		C3D_API Submesh( Scene & p_scene, Mesh & p_mesh, uint32_t p_id = 1 );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Submesh();
		/**
		 *\~english
		 *\brief		Initialises the submesh
		 *\~french
		 *\brief		Initialise le sous-maillage
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Cleans the submesh
		 *\~french
		 *\brief		Nettoie le sous-maillage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Computes the containers (cube and sphere)
		 *\~french
		 *\brief		Calcule les conteneurs (cube et sphère)
		 */
		C3D_API void ComputeContainers();
		/**
		 *\~english
		 *\return		The faces number
		 *\~french
		 *\return		Le nombre de faces de ce submesh
		 */
		C3D_API uint32_t GetFaceCount()const;
		/**
		 *\~english
		 *\return		The points count
		 *\~french
		 *\return		Le nombre de vertices de ce sous-maillage
		 */
		C3D_API uint32_t GetPointsCount()const;
		/**
		 *\~english
		 *\brief		Tests if the given Point3r is in mine
		 *\param[in]	p_vertex	The vertex to test
		 *\param[in]	p_precision	The comparison precision
		 *\return		The index of the vertex equal to parameter, -1 if not found
		 *\~french
		 *\brief		Teste si le point donné fait partie de ceux de ce sous-maillage
		 *\param[in]	p_vertex	Le point à tester
		 *\param[in]	p_precision	La précision de comparaison
		 *\return		L'index du point s'il a été trouvé, -1 sinon
		 */
		C3D_API int IsInMyPoints( Castor::Point3r const & p_vertex, double p_precision );
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
		C3D_API BufferElementGroupSPtr AddPoint( real x, real y, real z );
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
		C3D_API BufferElementGroupSPtr AddPoint( Castor::Point3r const & p_v );
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
		C3D_API BufferElementGroupSPtr AddPoint( real * p_v );
		/**
		 *\~english
		 *\brief		Creates and adds a vertex to my list.
		 *\param[in]	p_v	The vertex.
		 *\return		The created vertex.
		 *\~french
		 *\brief		Crée un Vertex et l'ajoute à la liste.
		 *\param[in]	p_v	Le sommet.
		 *\return		Le vertex créé.
		 */
		C3D_API BufferElementGroupSPtr AddPoint( InterleavedVertex const & p_v );
		/**
		 *\~english
		 *\brief		Adds a points list to my list.
		 *\param[in]	p_begin	The vertices data begin.
		 *\param[in]	p_end	The vertices data end.
		 *\~french
		 *\brief		Ajoute des points à la liste.
		 *\param[in]	p_begin	Le début des données de sommets.
		 *\param[in]	p_end	La fin des données de sommets.
		 */
		C3D_API void AddPoints( InterleavedVertex const * const p_begin, InterleavedVertex const * const p_end );
		/**
		 *\~english
		 *\brief		Adds bone datas.
		 *\param[in]	p_begin	The bones data begin.
		 *\param[in]	p_end	The bones data end.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	p_begin	Le début des données de bones.
		 *\param[in]	p_end	La fin des données de bones.
		 */
		C3D_API void AddBoneDatas( VertexBoneData const * const p_begin, VertexBoneData const * const p_end );
		/**
		 *\~english
		 *\brief		Clears this submesh's face array
		 *\~french
		 *\brief		Vide le tableau de faces
		 */
		C3D_API void ClearFaces();
		/**
		 *\~english
		 *\brief		Creates and adds a face to the submesh
		 *\param[in]	a			The first face's vertex index
		 *\param[in]	b			The second face's vertex index
		 *\param[in]	c			The third face's vertex index
		 *\return		The created face
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage
		 *\param[in]	a			L'index du premier vertex
		 *\param[in]	b			L'index du second vertex
		 *\param[in]	c			L'index du troisième vertex
		 *\return		La face créée
		 */
		C3D_API Face AddFace( uint32_t a, uint32_t b, uint32_t c );
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh
		 *\param[in]	p_begin	The faces data begin.
		 *\param[in]	p_end	The faces data end.
		 *\~french
		 *\brief		Crée et ajoute des faces au sous-maillage
		 *\param[in]	p_begin	Le début des données de faces.
		 *\param[in]	p_end	La fin des données de faces.
		 */
		C3D_API void AddFaceGroup( FaceIndices const * const p_begin, FaceIndices const * const p_end );
		/**
		 *\~english
		 *\brief		Creates and adds a quad face to the submesh
		 *\param[in]	a		The first face's vertex index
		 *\param[in]	b		The second face's vertex index
		 *\param[in]	c		The third face's vertex index
		 *\param[in]	d		The fourth face's vertex index
		 *\param[in]	p_minUV	The UV of the bottom left corner
		 *\param[in]	p_maxUV	The UV of the top right corner
		 *\return		The created face
		 *\~french
		 *\brief		Crée et ajoute une face à 4 côtés au sous-maillage
		 *\param[in]	a		L'index du premier vertex
		 *\param[in]	b		L'index du second vertex
		 *\param[in]	c		L'index du troisième vertex
		 *\param[in]	d		L'index du quatrième vertex
		 *\param[in]	p_minUV	L'UV du coin bas gauche
		 *\param[in]	p_maxUV	L'UV du coin haut droit
		 *\return		La face créée
		 */
		C3D_API void AddQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d, Castor::Point3r const & p_minUV = Castor::Point3r(), Castor::Point3r const & p_maxUV = Castor::Point3r( 1, 1, 1 ) );
		/**
		 *\~english
		 *\brief		Recreates the buffers.
		 *\~french
		 *\brief		Recrée les tampons.
		 */
		C3D_API void ResetGpuBuffers();
		/**
		 *\~english
		 *\brief		Recreates the Matrix buffers.
		 *\~french
		 *\brief		Recrée le tampon de matrices.
		 */
		C3D_API void ResetMatrixBuffers();
		/**
		 *\~english
		 *\brief		Draws the submesh.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to draw this submesh.
		 *\~french
		 *\brief		Dessine le sous-maillage.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner ce sous-maillage.
		 */
		C3D_API void Draw( GeometryBuffers const & p_geometryBuffers );
		/**
		 *\~english
		 *\brief		Draws the submesh.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to draw this submesh.
		 *\param[in]	p_count				The instances count.
		 *\~french
		 *\brief		Dessine le sous-maillage.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner ce sous-maillage.
		 *\param[in]	p_count				Le nombre d'instances.
		 */
		C3D_API void DrawInstanced( GeometryBuffers const & p_geometryBuffers, uint32_t p_count );
		/**
		 *\~english
		 *\brief		Creates faces from the points
		 *\remarks		This function assumes the points are sorted like triangles fan
		 *\~french
		 *\brief		Crée les faces à partir des points
		 *\remarks		Cette fonction suppose que les points sont tirés à la manière triangles fan
		 */
		C3D_API void ComputeFacesFromPolygonVertex();
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		C3D_API void ComputeNormals( bool p_reverted = false );
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face
		 *\param[in]	p_face	The face
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée
		 *\param[in]	p_face	La face
		 */
		C3D_API void ComputeNormals( Face const & p_face );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the given face
		 *\param[in]	p_face	The face
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée
		 *\param[in]	p_face	La face
		 */
		C3D_API void ComputeTangents( Face const & p_face );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh
		 *\remarks		This function supposes the normals are defined
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage
		 *\remarks		Cette fonction suppose que les normales sont définies
		 */
		C3D_API void ComputeTangentsFromNormals();
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh
		 *\remarks		This function supposes bitangents and normals are defined
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage
		 *\remarks		Cette fonction suppose que les bitangentes et les normales sont définies
		 */
		C3D_API void ComputeTangentsFromBitangents();
		/**
		 *\~english
		 *\brief		Computes bitangent for each vertex of the submesh
		 *\remarks		This function supposes the tangents and normals are defined
		 *\~french
		 *\brief		Calcule la bitangente pour chaque vertex du sous-maillage
		 *\remarks		Cette fonction suppose que les tangentes et les normales sont définies
		 */
		C3D_API void ComputeBitangents();
		/**
		 *\~english
		 *\brief		Sorts the face from farthest to nearest from the camera
		 *\param[in]	p_cameraPosition	The camera position, relative to submesh
		 *\~french
		 *\brief		Trie les faces des plus éloignées aux plus proches de la caméra
		 *\param[in]	p_cameraPosition	La position de la caméra, relative au sous-maillage
		 */
		C3D_API void SortByDistance( Castor::Point3r const & p_cameraPosition );
		/**
		 *\~english
		 *\brief		Increments instance count.
		 *\param[in]	p_material	The material for which the instance count is incremented.
		 *\return		The previous instance count.
		 *\~french
		 *\brief		Incrémente le compte d'instances.
		 *\param[in]	p_material	Le matériau pour lequel le compte est incrémenté.
		 *\return		Le compte précédent.
		 */
		C3D_API uint32_t Ref( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Decrements instance count.
		 *\param[in]	p_material	The material for which the instance count is decremented.
		 *\return		The previous instance count.
		 *\~french
		 *\brief		Décrémente le compte d'instances.
		 *\param[in]	p_material	Le matériau pour lequel le compte est décrémenté.
		 *\return		Le compte précédent.
		 */
		C3D_API uint32_t UnRef( MaterialSPtr p_material );
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
		C3D_API uint32_t GetRefCount( MaterialSPtr p_material )const;
		/**
		 *\~english
		 *\return		The topology.
		 *\~french
		 *\return		La topologie.
		 */
		C3D_API Topology GetTopology()const;
		/**
		 *\~english
		 *\brief		Sets the topology.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la topologie.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void SetTopology( Topology p_value );
		/**
		 *\~english
		 *\brief		Retrieves a GeometryBuffers for given program.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Récupère un GeometryBuffers pour le programme donné.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API GeometryBuffersSPtr GetGeometryBuffers( ShaderProgram const & p_program );
		/**
		 *\~english
		 *\return		The animated status.
		 *\~french
		 *\return		Le statut de sous-maillage animé.
		 */
		C3D_API void SetAnimated( bool p_animated );
		/**
		 *\~english
		 *\brief		Adds a points list to my list
		 *\param[in]	p_vertices	The vertices
		 *\~french
		 *\brief		Ajoute des points à la liste
		 *\param[in]	p_vertices	Les vertices
		 */
		inline void AddPoints( std::vector< InterleavedVertex > const & p_vertices )
		{
			AddPoints( p_vertices.data(), p_vertices.data() + p_vertices.size() );
		}
		/**
		 *\~english
		 *\brief		Adds a points list to my list
		 *\param[in]	p_vertices	The vertices
		 *\~french
		 *\brief		Ajoute des points à la liste
		 *\param[in]	p_vertices	Les vertices
		 */
		template< size_t Count >
		inline void AddPoints( std::array< InterleavedVertex, Count > const & p_vertices )
		{
			AddPoints( p_vertices.data(), p_vertices.data() + p_vertices.size() );
		}
		/**
		 *\~english
		 *\brief		Adds bone datas.
		 *\param[in]	p_boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	p_boneData	Les données de bones.
		 */
		inline void AddBoneDatas( std::vector< VertexBoneData > const & p_boneData )
		{
			AddBoneDatas( p_boneData.data(), p_boneData.data() + p_boneData.size() );
		}
		/**
		 *\~english
		 *\brief		Adds bone datas.
		 *\param[in]	p_boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	p_boneData	Les données de bones.
		 */
		template< size_t Count >
		inline void AddBoneDatas( std::array< VertexBoneData, Count > const & p_boneData )
		{
			AddBoneDatas( p_boneData.data(), p_boneData.data() + p_boneData.size() );
		}
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh
		 *\param[in]	p_faces	The faces
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage
		 *\param[in]	p_faces	Les faces
		 */
		inline void AddFaceGroup( std::vector< FaceIndices > const & p_faces )
		{
			AddFaceGroup( p_faces.data(), p_faces.data() + p_faces.size() );
		}
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh
		 *\param[in]	p_faces	The faces
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage
		 *\param[in]	p_faces	Les faces
		 */
		template< size_t Count >
		inline void AddFaceGroup( std::array< FaceIndices, Count > const & p_faces )
		{
			AddFaceGroup( p_faces.data(), p_faces.data() + p_faces.size() );
		}
		/**
		 *\~english
		 *\return		The skeleton.
		 *\~french
		 *\return		Le squelette.
		 */
		inline SkeletonSPtr GetSkeleton()const
		{
			return GetParent().GetSkeleton();
		}
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_mat	The new value
		 *\~french
		 *\brief		Définit le material
		 *\param[in]	p_mat	La nouvelle valeur
		 */
		inline void SetDefaultMaterial( MaterialSPtr p_mat )
		{
			m_defaultMaterial = p_mat;
		}
		/**
		 *\~english
		 *\brief		Retrieves the point at given index
		 *\param[in]	p_index	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le point à l'index donné
		 *\param[in]	p_index	L'index
		 *\return		La valeur
		 */
		inline BufferElementGroupSPtr operator[]( uint32_t p_index )const
		{
			REQUIRE( p_index < m_points.size() );
			return m_points[p_index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the point at given index
		 *\param[in]	p_index	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le point à l'index donné
		 *\param[in]	p_index	L'index
		 *\return		La valeur
		 */
		inline BufferElementGroupSPtr GetPoint( uint32_t p_index )const
		{
			REQUIRE( p_index < m_points.size() );
			return m_points[p_index];
		}
		/**
		 *\~english
		 *\return		The material.
		 *\~french
		 *\return		Le matériau.
		 */
		inline MaterialSPtr GetDefaultMaterial()const
		{
			return m_defaultMaterial.lock();
		}
		/**
		 *\~english
		 *\return		The cube bounding box.
		 *\~french
		 *\return		La bounding box cube.
		 */
		inline Castor::CubeBox const & GetCollisionBox()const
		{
			return m_box;
		}
		/**
		 *\~english
		 *\return		The cube bounding box.
		 *\~french
		 *\return		La bounding box cube.
		 */
		inline Castor::CubeBox & GetCollisionBox()
		{
			return m_box;
		}
		/**
		 *\~english
		 *\return		The sphere bounding box.
		 *\~french
		 *\return		La bounding box sphère.
		 */
		inline Castor::SphereBox const & GetCollisionSphere()const
		{
			return m_sphere;
		}
		/**
		 *\~english
		 *\return		The sphere bounding box.
		 *\~french
		 *\return		La bounding box sphère.
		 */
		inline Castor::SphereBox & GetCollisionSphere()
		{
			return m_sphere;
		}
		/**
		 *\~english
		 *\return		The points array.
		 *\~french
		 *\return		Le tableau de points.
		 */
		inline VertexPtrArray const & GetPoints()const
		{
			return m_points;
		}
		/**
		 *\~english
		 *\return		The points array.
		 *\~french
		 *\return		Le tableau de points.
		 */
		inline VertexPtrArray & GetPoints()
		{
			return m_points;
		}
		/**
		 *\~english
		 *\brief		Retrieves the face at given index
		 *\param[in]	p_index	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère récupère la face à l'index donné
		 *\param[in]	p_index	L'index
		 *\return		La valeur
		 */
		inline Face const & GetFace( uint32_t p_index )const
		{
			REQUIRE( p_index < m_faces.size() );
			return m_faces[p_index];
		}
		/**
		 *\~english
		 *\return		The faces array.
		 *\~french
		 *\return		Le tableau de faces.
		 */
		inline FaceArray const & GetFaces()const
		{
			return m_faces;
		}
		/**
		 *\~english
		 *\return		The faces array.
		 *\~french
		 *\return		Le tableau de faces.
		 */
		inline FaceArray & GetFaces()
		{
			return m_faces;
		}
		/**
		 *\~english
		 *\return		The VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer.
		 */
		inline bool HasVertexBuffer()const
		{
			return bool( m_vertexBuffer );
		}
		/**
		 *\~english
		 *\return		The IndexBuffer.
		 *\~french
		 *\return		L'IndexBuffer.
		 */
		inline bool HasAnimationBuffer()const
		{
			return bool( m_animBuffer );
		}
		/**
		 *\~english
		 *\return		The IndexBuffer.
		 *\~french
		 *\return		L'IndexBuffer.
		 */
		inline bool HasIndexBuffer()const
		{
			return bool( m_indexBuffer );
		}
		/**
		 *\~english
		 *\return		The bones VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer des bones.
		 */
		inline bool HasBonesBuffer()const
		{
			return bool( m_bonesBuffer );
		}
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline bool HasMatrixBuffer()const
		{
			return bool( m_matrixBuffer );
		}
		/**
		 *\~english
		 *\return		The VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer.
		 */
		inline VertexBuffer const & GetVertexBuffer()const
		{
			return *m_vertexBuffer;
		}
		/**
		 *\~english
		 *\return		The VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer.
		 */
		inline VertexBuffer & GetVertexBuffer()
		{
			return *m_vertexBuffer;
		}
		/**
		 *\~english
		 *\return		The VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer.
		 */
		inline VertexBuffer const & GetAnimationBuffer()const
		{
			return *m_animBuffer;
		}
		/**
		 *\~english
		 *\return		The VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer.
		 */
		inline VertexBuffer & GetAnimationBuffer()
		{
			return *m_animBuffer;
		}
		/**
		 *\~english
		 *\return		The IndexBuffer.
		 *\~french
		 *\return		L'IndexBuffer.
		 */
		inline IndexBuffer const & GetIndexBuffer()const
		{
			return *m_indexBuffer;
		}
		/**
		 *\~english
		 *\return		The IndexBuffer.
		 *\~french
		 *\return		L'IndexBuffer.
		 */
		inline IndexBuffer & GetIndexBuffer()
		{
			return *m_indexBuffer;
		}
		/**
		 *\~english
		 *\return		The bones VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer des bones.
		 */
		inline VertexBuffer const & GetBonesBuffer()const
		{
			return *m_bonesBuffer;
		}
		/**
		 *\~english
		 *\return		The bones VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer des bones.
		 */
		inline VertexBuffer & GetBonesBuffer()
		{
			return *m_bonesBuffer;
		}
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline VertexBuffer const & GetMatrixBuffer()const
		{
			return *m_matrixBuffer;
		}
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline VertexBuffer & GetMatrixBuffer()
		{
			return *m_matrixBuffer;
		}
		/**
		 *\~english
		 *\return		The initialisation status.
		 *\~french
		 *\return		Le statut d'initialisation.
		 */
		inline bool	IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\return		The parent mesh.
		 *\~french
		 *\return		Le maillage parent.
		 */
		inline Mesh const & GetParent()const
		{
			return m_parentMesh;
		}
		/**
		 *\~english
		 *\return		The parent mesh.
		 *\~french
		 *\return		Le maillage parent.
		 */
		inline Mesh & GetParent()
		{
			return m_parentMesh;
		}
		/**
		 *\~english
		 *\return		The shader program flags.
		 *\~french
		 *\return		Les indicateurs de shader.
		 */
		inline ProgramFlags const & GetProgramFlags()const
		{
			return m_programFlags;
		}
		/**
		 *\~english
		 *\return		The submesh ID.
		 *\~french
		 *\return		L'ID du sous-maillage.
		 */
		inline uint32_t GetId()const
		{
			return m_id;
		}
		/**
		 *\~english
		 *\brief		Sets the submesh to be updated.
		 *\~french
		 *\return		Dit que le sous-maillage doit être mis à jour.
		 */
		inline void NeedUpdate()
		{
			m_dirty = true;
		}
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh
		 *\param[in]	p_faces	The faces
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage
		 *\param[in]	p_faces	Les faces
		 */
		template< uint32_t Count > void AddFaceGroup( FaceIndices( & p_faces )[Count] )
		{
			AddFaceGroup( p_faces, Count );
		}

	private:
		Face & DoAddFace( const Face & p_face );
		void DoCreateBuffers();
		void DoDestroyBuffers();
		void DoGenerateBuffers();
		void DoGenerateVertexBuffer();
		void DoGenerateAnimBuffer();
		void DoGenerateIndexBuffer();
		void DoGenerateBonesBuffer();
		void DoGenerateMatrixBuffer( uint32_t p_count );
		void DoInitialiseGeometryBuffers( GeometryBuffersSPtr p_geometryBuffers );

	private:
		//!\~english	The submesh ID.
		//!\~french		L'id du sbmesh.
		uint32_t m_id{ 0 };
		//!\~english	Tells if normals exist or need to be computed.
		//!\~french		Dit si les normales existent ou doivent être calculées.
		bool m_hasNormals{ false };
		//!\~english	The shader program flags.
		//!\~french		Les indicateurs pour le shader.
		ProgramFlags m_programFlags{ 0u };
		//!\~english	Tells the VBOs has been initialised.
		//!\~french		Dit que les VBOs ont été initialisés.
		bool m_initialised{ false };
		//!\~english	Tells the VAO needs reininitialisation.
		//!\~french		Dit que le VAO a besoin d'être réinitialisé.
		bool m_dirty{ true };
		//!\~english	The submesh instances count.
		//!\~french		Le nombre d'instances du sous-maillage.
		std::map< MaterialSPtr, uint32_t > m_instanceCount;
		//!\~english	Vertex elements declaration, deduced from points.
		//!\~french		Déclaration des éléments d'un sommet, déduite à partir des points.
		BufferDeclaration m_layout;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The animated vertex buffer.
		//!\~french		Le tampon de sommets animés.
		VertexBufferSPtr m_animBuffer;
		//!\~english	The index buffer.
		//!\~french		Le tampon d'indices.
		IndexBufferSPtr m_indexBuffer;
		//!\~english	The bone data buffer (animation).
		//!\~french		Le tampon de données de bones (animation).
		VertexBufferSPtr m_bonesBuffer;
		//!\~english	The matrix buffer (instantiation).
		//!\~french		Le tampon de matrices (instanciation).
		VertexBufferSPtr m_matrixBuffer;
		//!\~english	The faces in the submesh.
		//!\~french		Le tableau de faces.
		FaceArray m_faces;
		//!\~english	The Material assigned at creation.
		//!\~french		Le Materiau affecté à la création.
		MaterialWPtr m_defaultMaterial;
		//!\~english	The combo box container.
		//!\~french		Le conteneur boîte.
		Castor::CubeBox m_box;
		//!\~english	The spheric container.
		//!\~french		Le conteneur sphère.
		Castor::SphereBox m_sphere;
		//!\~english	The vertex data array.
		//!\~french		Le tableau de données des sommets.
		BytePtrList m_pointsData;
		//!\~english	The vertex pointer array.
		//!\~french		Le tableau de sommets.
		VertexPtrArray m_points;
		//!\~english	The bones data array.
		//!\~french		Le tableau de données des bones.
		BytePtrList m_bonesData;
		//!\~english	The bones pointer array.
		//!\~french		Le tableau de bones.
		VertexPtrArray m_bones;
		//!\~english	The transformed camera position at last sort.
		//!\~french		La position transformée de la caméra au dernier tri.
		Castor::Point3r m_cameraPosition;
		//!\~english	The parent mesh.
		//!\~french		Le maillage parent.
		Mesh & m_parentMesh;
		//!\~english	The GeometryBuffers with which this submesh is compatible.
		//!\~french		Les GeometryBuffers avec lesquel ce sous-maillage est compatible.
		std::vector< GeometryBuffersSPtr > m_geometryBuffers;

		friend class GeometryBuffers;
		friend class BinaryWriter< Submesh >;
		friend class BinaryParser< Submesh >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for Submesh.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour Submesh.
	*/
	template<>
	struct ChunkTyper< Submesh >
	{
		static ChunkType const Value = ChunkType::eSubmesh;
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
	class BinaryWriter< Submesh >
		: public BinaryWriterBase< Submesh >
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
		C3D_API bool DoWrite( Submesh const & p_obj )override;
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
	class BinaryParser< Submesh >
		: public BinaryParserBase< Submesh >
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
		C3D_API bool DoParse( Submesh & p_obj )override;
	};
}

#endif
