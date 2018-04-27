/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Submesh_H___
#define ___C3D_Submesh_H___

#include "Castor3DPrerequisites.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/VertexGroup.hpp"
#include "Mesh/Skeleton/VertexBoneData.hpp"

#include <Design/OwnedBy.hpp>

#include <Buffer/VertexBuffer.hpp>

namespace castor3d
{
	template< typename T >
	struct SubmeshComponentAdder
	{
		static inline void add( std::shared_ptr< T > component
			, Submesh & submesh );
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The submesh representation.
	\remarks	A submesh holds its buffers (vertex, normals and texture) and its combobox.
	\~french
	\brief		Representation d'un sous-maillage.
	\remarks	Un sous-maillage est sous partie d'un maillage. Il possede ses propres tampons (vertex, normales et texture coords) et ses combobox.
	*/
	class Submesh
		: public castor::OwnedBy< Scene >
		, public std::enable_shared_from_this< Submesh >
	{
	private:
		DECLARE_LIST( castor::ByteArray, BytePtr );
		template< typename T > friend struct SubmeshComponentAdder;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	mesh	The parent mesh.
		 *\param[in]	scene	The scene.
		 *\param[in]	id	The submesh ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	mesh	Le mesh parent.
		 *\param[in]	scene	La scène.
		 *\param[in]	id	L'ID du sous-maillage.
		 */
		C3D_API Submesh( Scene & scene, Mesh & mesh, uint32_t id = 1 );
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
		C3D_API void initialise();
		/**
		*\~english
		*\brief		Cleans the submesh
		*\~french
		*\brief		Nettoie le sous-maillage
		*/
		C3D_API void cleanup();
		/**
		*\~english
		*\brief		Updates the buffers.
		*\~french
		*\brief		Met à jour les tampons.
		*/
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Computes the containers (cube and sphere)
		 *\~french
		 *\brief		Calcule les conteneurs (cube et sphère)
		 */
		C3D_API void computeContainers();
		/**
		 *\~english
		 *\brief		Updates containers from given bounding box.
		 *\~french
		 *\brief		Met à jour les conteneurs depuis la bounding box donnée
		 */
		C3D_API void updateContainers( castor::BoundingBox const & boundingBox );
		/**
		 *\~english
		 *\return		The faces number
		 *\~french
		 *\return		Le nombre de faces de ce submesh
		 */
		C3D_API uint32_t getFaceCount()const;
		/**
		 *\~english
		 *\return		The points count
		 *\~french
		 *\return		Le nombre de vertices de ce sous-maillage
		 */
		C3D_API uint32_t getPointsCount()const;
		/**
		 *\~english
		 *\brief		Tests if the given Point3r is in mine
		 *\param[in]	position	The vertex to test
		 *\param[in]	precision	The comparison precision
		 *\return		The index of the vertex equal to parameter, -1 if not found
		 *\~french
		 *\brief		Teste si le point donné fait partie de ceux de ce sous-maillage
		 *\param[in]	position	Le point à tester
		 *\param[in]	precision	La précision de comparaison
		 *\return		L'index du point s'il a été trouvé, -1 sinon
		 */
		C3D_API int isInMyPoints( castor::Point3r const & position, double precision );
		/**
		 *\~english
		 *\brief		Creates and Adds a vertex to my list
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
		C3D_API InterleavedVertex addPoint( real x, real y, real z );
		/**
		 *\~english
		 *\brief		Adds a vertex to my list
		 *\param[in]	value	The vertex to add
		 *\return		The vertex
		 *\~french
		 *\brief		Crée un Vertex à partir du point donné et l'ajoute à la liste
		 *\param[in]	value	Le point
		 *\return		Le vertex créé
		 */
		C3D_API InterleavedVertex addPoint( castor::Point3r const & value );
		/**
		 *\~english
		 *\brief		Creates and Adds a vertex to my list
		 *\param[in]	value	The vertex coordinates
		 *\return		The created vertex
		 *\~french
		 *\brief		Crée un Vertex à partir des coordonnées données et l'ajoute à la liste
		 *\param[in]	value	Les coordonnées du point
		 *\return		Le vertex créé
		 */
		C3D_API InterleavedVertex addPoint( real * value );
		/**
		 *\~english
		 *\brief		Adds a vertex to the list.
		 *\param[in]	value	The vertex.
		 *\~french
		 *\brief		Ajoute un sommet à la liste.
		 *\param[in]	value	Le sommet.
		 */
		C3D_API void addPoint( InterleavedVertex const & value );
		/**
		 *\~english
		 *\brief		Adds a points list to my list.
		 *\param[in]	begin	The vertices data begin.
		 *\param[in]	end	The vertices data end.
		 *\~french
		 *\brief		Ajoute des points à la liste.
		 *\param[in]	begin	Le début des données de sommets.
		 *\param[in]	end	La fin des données de sommets.
		 */
		C3D_API void addPoints( InterleavedVertex const * const begin
			, InterleavedVertex const * const end );
		/**
		 *\~english
		 *\brief		Generates normals and tangents
		 *\~french
		 *\brief		Génère les normales et les tangentes
		 */
		C3D_API void computeNormals( bool reverted = false );
		/**
		 *\~english
		 *\brief		Sorts the face from farthest to nearest from the camera
		 *\param[in]	cameraPosition	The camera position, relative to submesh
		 *\~french
		 *\brief		Trie les faces des plus éloignées aux plus proches de la caméra
		 *\param[in]	cameraPosition	La position de la caméra, relative au sous-maillage
		 */
		C3D_API void sortByDistance( castor::Point3r const & cameraPosition );
		/**
		 *\~english
		 *\return		The shader program flags.
		 *\~french
		 *\return		Les indicateurs de shader.
		 */
		C3D_API ProgramFlags getProgramFlags( MaterialSPtr material )const;
		/**
		 *\~english
		 *\brief		Sets the material.
		 *\param[in]	oldMaterial	The old material.
		 *\param[in]	newMaterial	The new material.
		 *\param[in]	update		Tells if the buffers must be updated.
		 *\return		\p true si le matériau a changé.
		 *\~french
		 *\brief		Définit le materiau.
		 *\param[in]	oldMaterial	Le matériau précédent.
		 *\param[in]	newMaterial	Le nouveau matériau.
		 *\param[in]	update		Dit si les tampons doivent être mis à jour.
		 *\return		\p true if the material is changed.
		 */
		C3D_API void setMaterial( MaterialSPtr oldMaterial
			, MaterialSPtr newMaterial
			, bool update );
		/**
		*\~english
		*\return		The geometry buffers for given material.
		*\~french
		*\return		Les tampons de géométrie associés au materiau donné.
		*/
		C3D_API GeometryBuffers const & getGeometryBuffers( MaterialSPtr material )const;
		/**
		 *\~english
		 *\brief		Adds a points list to my list
		 *\param[in]	vertices	The vertices
		 *\~french
		 *\brief		Ajoute des points à la liste
		 *\param[in]	vertices	Les vertices
		 */
		inline void addPoints( std::vector< InterleavedVertex > const & vertices );
		/**
		 *\~english
		 *\brief		Adds a points list to my list
		 *\param[in]	vertices	The vertices
		 *\~french
		 *\brief		Ajoute des points à la liste
		 *\param[in]	vertices	Les vertices
		 */
		template< size_t Count >
		inline void addPoints( std::array< InterleavedVertex, Count > const & vertices );
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	material	The new value
		 *\~french
		 *\brief		Définit le material
		 *\param[in]	material	La nouvelle valeur
		 */
		inline void setDefaultMaterial( MaterialSPtr material );
		/**
		 *\~english
		 *\brief		Sets the submesh to be updated.
		 *\~french
		 *\brief		Dit que le sous-maillage doit être mis à jour.
		 */
		inline void needsUpdate();
		/**
		 *\~english
		 *\brief		Sets the index mapping.
		 *\param[in]	mapping	The mapping.
		 *\~french
		 *\brief		Définit le mappage d'indices.
		 *\param[in]	mapping	Le mappage.
		 */
		inline void setIndexMapping( IndexMappingSPtr mapping );
		/**
		 *\~english
		 *\return		The index mapping.
		 *\~french
		 *\return		Le mappage d'indices.
		 */
		inline IndexMapping const & getIndexMapping()const;
		/**
		 *\~english
		 *\brief		Adds a component.
		 *\param[in]	name		The component name.
		 *\param[in]	component	The component.
		 *\~french
		 *\brief		Ajoute un composant.
		 *\param[in]	name		Le nom du composant.
		 *\param[in]	component	Le composant.
		 */
		inline void addComponent( castor::String const & name
			, SubmeshComponentSPtr component );
		/**
		 *\~english
		 *\brief		Adds a component.
		 *\param[in]	component	The component.
		 *\~french
		 *\brief		Ajoute un composant.
		 *\param[in]	component	Le composant.
		 */
		template< typename T >
		inline void addComponent( std::shared_ptr< T > component );
		/**
		 *\~english
		 *\brief		Sets the topology.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la topologie.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setTopology( renderer::PrimitiveTopology value );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline SkeletonSPtr getSkeleton()const;
		inline InterleavedVertex const & operator[]( uint32_t index )const;
		inline InterleavedVertex & operator[]( uint32_t index );
		inline InterleavedVertex const & getPoint( uint32_t index )const;
		inline InterleavedVertex & getPoint( uint32_t index );
		inline MaterialSPtr getDefaultMaterial()const;
		inline castor::BoundingBox const & getBoundingBox()const;
		inline castor::BoundingBox & getBoundingBox();
		inline castor::BoundingSphere const & getBoundingSphere()const;
		inline castor::BoundingSphere & getBoundingSphere();
		inline InterleavedVertexArray const & getPoints()const;
		inline InterleavedVertexArray & getPoints();
		inline renderer::VertexBuffer< InterleavedVertex > const & getVertexBuffer()const;
		inline renderer::VertexBuffer< InterleavedVertex > & getVertexBuffer();
		inline renderer::VertexLayout const & getVertexLayout()const;
		inline renderer::Buffer< uint32_t > const & getIndexBuffer()const;
		inline renderer::Buffer< uint32_t > & getIndexBuffer();
		inline bool isInitialised()const;
		inline Mesh const & getParent()const;
		inline Mesh & getParent();
		inline uint32_t getId()const;
		inline bool hasComponent( castor::String const & name )const;
		inline SubmeshComponentSPtr getComponent( castor::String const & name )const;
		template< typename T >
		inline std::shared_ptr< T > getComponent()const;
		inline InstantiationComponent & getInstantiation();
		inline InstantiationComponent const & getInstantiation()const;
		inline BonesInstantiationComponent & getInstantiatedBones();
		inline BonesInstantiationComponent const & getInstantiatedBones()const;
		inline SubmeshComponentStrMap const & getComponents()const;
		inline renderer::PrimitiveTopology getTopology()const;

	private:
		void doGenerateVertexBuffer();

	public:
		static uint32_t constexpr Position = 0u;
		static uint32_t constexpr Normal = 1u;
		static uint32_t constexpr Tangent = 2u;
		static uint32_t constexpr Bitangent = 3u;
		static uint32_t constexpr Texture = 4u;

	private:
		Mesh & m_parentMesh;
		uint32_t m_id;
		MaterialWPtr m_defaultMaterial;
		castor::BoundingBox m_box;
		castor::BoundingSphere m_sphere;
		InterleavedVertexArray m_points;
		SubmeshComponentStrMap m_components;
		InstantiationComponentSPtr m_instantiation;
		BonesInstantiationComponentSPtr m_instantiatedBones;
		IndexMappingSPtr m_indexMapping;
		ProgramFlags m_programFlags{ 0u };
		bool m_generated{ false };
		bool m_initialised{ false };
		bool m_dirty{ true };
		renderer::PrimitiveTopology m_topology{ renderer::PrimitiveTopology::eTriangleList };
		renderer::VertexBufferPtr< InterleavedVertex > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::BufferPtr< uint32_t > m_indexBuffer;
		mutable std::map< MaterialSPtr, GeometryBuffers > m_geometryBuffers;

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
		C3D_API bool doWrite( Submesh const & p_obj )override;
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
		C3D_API bool doParse( Submesh & p_obj )override;
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
		C3D_API bool doParse_v1_3( Submesh & p_obj )override;
	};
}

#include "Submesh.inl"

#endif
