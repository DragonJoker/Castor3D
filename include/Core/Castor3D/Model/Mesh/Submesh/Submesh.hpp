/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Submesh_H___
#define ___C3D_Submesh_H___

#include "Component/ComponentModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Model/VertexGroup.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <unordered_map>

namespace castor3d
{
	template< typename T >
	struct SubmeshComponentAdder
	{
		static inline void add( std::shared_ptr< T > component
			, Submesh & submesh );
	};

	class Submesh
		: public castor::OwnedBy< Mesh >
		, public std::enable_shared_from_this< Submesh >
	{
	private:
		CU_DeclareList( castor::ByteArray, BytePtr );
		template< typename T >
		friend struct SubmeshComponentAdder;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	mesh	The parent mesh.
		 *\param[in]	id		The submesh ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	mesh	Le mesh parent.
		 *\param[in]	id		L'ID du sous-maillage.
		 */
		C3D_API explicit Submesh( Mesh & mesh
			, uint32_t id = 1 );
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
		C3D_API void initialise( RenderDevice const & device );
		/**
		*\~english
		*\brief		Cleans the submesh
		*\~french
		*\brief		Nettoie le sous-maillage
		*/
		C3D_API void cleanup( RenderDevice const & device );
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
		 *\brief		Tests if the given Point3f is in mine
		 *\param[in]	position	The vertex to test
		 *\param[in]	precision	The comparison precision
		 *\return		The index of the vertex equal to parameter, -1 if not found
		 *\~french
		 *\brief		Teste si le point donné fait partie de ceux de ce sous-maillage
		 *\param[in]	position	Le point à tester
		 *\param[in]	precision	La précision de comparaison
		 *\return		L'index du point s'il a été trouvé, -1 sinon
		 */
		C3D_API int isInMyPoints( castor::Point3f const & position, double precision );
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
		C3D_API InterleavedVertex addPoint( float x, float y, float z );
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
		C3D_API InterleavedVertex addPoint( castor::Point3f const & value );
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
		C3D_API InterleavedVertex addPoint( float * value );
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
		C3D_API void sortByDistance( castor::Point3f const & cameraPosition );
		/**
		 *\~english
		 *\return		The shader program flags.
		 *\~french
		 *\return		Les indicateurs de shader.
		 */
		C3D_API ProgramFlags getProgramFlags( MaterialRPtr material )const;
		/**
		 *\~english
		 *\brief		Sets the material.
		 *\param[in]	oldMaterial	The old material.
		 *\param[in]	newMaterial	The new material.
		 *\param[in]	update		Tells if the buffers must be updated.
		 *\~french
		 *\brief		Définit le materiau.
		 *\param[in]	oldMaterial	Le matériau précédent.
		 *\param[in]	newMaterial	Le nouveau matériau.
		 *\param[in]	update		Dit si les tampons doivent être mis à jour.
		 */
		C3D_API void setMaterial( MaterialRPtr oldMaterial
			, MaterialRPtr newMaterial
			, bool update );
		/**
		*\~english
		*\return		The geometry buffers for given material.
		 *\param[in]	shaderFlags		The shader flags.
		 *\param[in]	programFlags	The program flags.
		 *\param[in]	material		The material.
		 *\param[in]	mask			The textures mask.
		 *\param[in]	forceTexcoords	\p true to force texture coordinates in the vertex layout.
		*\~french
		*\return		Les tampons de géométrie associés au materiau donné.
		 *\param[in]	shaderFlags		Les shader flags.
		 *\param[in]	programFlags	Les program flags.
		 *\param[in]	material		Les matériau.
		 *\param[in]	mask			Le masque de textures.
		 *\param[in]	forceTexcoords	\p true pour forcer les coordonnées de texture dans le vertex layout.
		*/
		C3D_API GeometryBuffers const & getGeometryBuffers( ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, MaterialRPtr material
			, TextureFlagsArray const & mask
			, bool forceTexcoords )const;
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
		inline void setDefaultMaterial( MaterialRPtr material );
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
		inline IndexMappingSPtr getIndexMapping()const;
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
		inline void setTopology( VkPrimitiveTopology value );
		/**
		 *\~english
		 *\brief		Disables scene update on modifications.
		 *\~french
		 *\brief		Désactive la mise à jour de la scène en cas de modification.
		 */
		inline void disableSceneUpdate();
		/**
		 *\~english
		 *\brief		Enables scene update on modifications.
		 *\param[in]	updateScene Tells if this call triggers a scene update.
		 *\~french
		 *\brief		Active la mise à jour de la scène en cas de modification.
		 *\param[in]	updateScene	Dit si cet appel déclenche une mise à jour de la scène.
		 */
		C3D_API void enableSceneUpdate( bool updateScene );
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
		inline MaterialRPtr getDefaultMaterial()const;
		inline castor::BoundingBox const & getBoundingBox()const;
		inline castor::BoundingBox & getBoundingBox();
		inline castor::BoundingSphere const & getBoundingSphere()const;
		inline castor::BoundingSphere & getBoundingSphere();
		inline InterleavedVertexArray const & getPoints()const;
		inline InterleavedVertexArray & getPoints();
		inline bool hasBufferOffsets()const;
		inline ObjectBufferOffset const & getBufferOffsets()const;
		inline bool isInitialised()const;
		inline bool isDynamic()const;
		inline Mesh const & getParent()const;
		inline Mesh & getParent();
		inline uint32_t getId()const;
		inline bool hasComponent( castor::String const & name )const;
		inline SubmeshComponentSPtr getComponent( castor::String const & name )const;
		template< typename T >
		inline std::shared_ptr< T > getComponent()const;
		inline InstantiationComponent & getInstantiation();
		inline InstantiationComponent const & getInstantiation()const;
		inline SubmeshComponentIDMap const & getComponents()const;
		inline VkPrimitiveTopology getTopology()const;

	private:
		void doFillVertexBuffer();

	public:
		static uint32_t constexpr Position = 0u;
		static uint32_t constexpr Normal = 1u;
		static uint32_t constexpr Tangent = 2u;
		static uint32_t constexpr Bitangent = 3u;
		static uint32_t constexpr Texture = 4u;

	private:
		uint32_t m_id;
		MaterialRPtr m_defaultMaterial;
		castor::BoundingBox m_box;
		castor::BoundingSphere m_sphere;
		InterleavedVertexArray m_points;
		SubmeshComponentIDMap m_components;
		InstantiationComponentSPtr m_instantiation;
		IndexMappingSPtr m_indexMapping;
		ProgramFlags m_programFlags{ 0u };
		bool m_dynamic{ false };
		bool m_generated{ false };
		bool m_initialised{ false };
		bool m_dirty{ true };
		VkPrimitiveTopology m_topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		ObjectBufferOffset m_bufferOffset;
		mutable std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > m_vertexLayouts;
		mutable std::unordered_map< size_t, GeometryBuffers > m_geometryBuffers;
		bool m_needsNormalsCompute{ false };
		bool m_disableSceneUpdate{ false };
		StagingDataUPtr m_staging;

		friend class BinaryWriter< Submesh >;
		friend class BinaryParser< Submesh >;
	};
}

#include "Submesh.inl"

#endif
