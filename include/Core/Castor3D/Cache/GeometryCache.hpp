/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryCache_H___
#define ___C3D_GeometryCache_H___

#include "Castor3D/Cache/ObjectCacheBase.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <set>

namespace castor3d
{
	C3D_API size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass );
	C3D_API size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass
		, uint32_t instanceMult );
	/**
	\~english
	\brief		Geometry cache.
	\~french
	\brief		Cache de Geometry.
	*/
	template<>
	class ObjectCache< Geometry, castor::String >
		: public ObjectCacheBase< Geometry, castor::String >
	{
	public:
		struct PoolsEntry
		{
			size_t hash;
			Geometry const & geometry;
			Submesh const & submesh;
			Pass const & pass;
			UniformBufferOffsetT< ModelUboConfiguration > modelUbo;
			UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo;
		};
		using MyObjectCache = ObjectCacheBase< Geometry, castor::String >;
		using MyObjectCacheTraits = typename MyObjectCacheType::MyObjectCacheTraits;
		using Element = typename MyObjectCacheType::Element;
		using Key = typename MyObjectCacheType::Key;
		using Collection = typename MyObjectCacheType::Collection;
		using LockType = typename MyObjectCacheType::LockType;
		using ElementPtr = typename MyObjectCacheType::ElementPtr;
		using Producer = typename MyObjectCacheType::Producer;
		using Initialiser = typename MyObjectCacheType::Initialiser;
		using Cleaner = typename MyObjectCacheType::Cleaner;
		using Merger = typename MyObjectCacheType::Merger;
		using Attacher = typename MyObjectCacheType::Attacher;
		using Detacher = typename MyObjectCacheType::Detacher;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	scene			The scene.
		 *\param[in]	rootNode		The root node.
		 *\param[in]	rootCameraNode	The cameras root node.
		 *\param[in]	rootObjectNode	The objects root node.
		 *\param[in]	produce			The element producer.
		 *\param[in]	initialise		The element initialiser.
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\param[in]	attach			The element attacher (to a scene node).
		 *\param[in]	detach			The element detacher (from a scene node).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	scene			La scène.
		 *\param[in]	rootNode		Le noeud racine.
		 *\param[in]	rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	rootObjectNode	Le noeud racine des objets.
		 *\param[in]	produce			Le créateur d'objet.
		 *\param[in]	initialise		L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 *\param[in]	attach			L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	detach			Le détacheur d'objet (d'un noeud de scène).
		 */
		C3D_API ObjectCache( Engine & engine
			, Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode
			, Producer && produce
			, Initialiser && initialise = Initialiser{}
			, Cleaner && clean = Cleaner{}
			, Merger && merge = Merger{}
			, Attacher && attach = Attacher{}
			, Detacher && detach = Detacher{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ObjectCache();

		C3D_API void registerPass( SceneRenderPass const & renderPass );
		C3D_API void unregisterPass( SceneRenderPass const * renderPass
			, uint32_t instanceMult );

		C3D_API void fillInfo( RenderInfo & info )const;
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\return		The UBOs for given geometry, submesh and pass.
		 *\param[in]	geometry		The geometry.
		 *\param[in]	submesh			The submesh.
		 *\param[in]	pass			The material pass.
		 *\param[in]	instanceMult	The instance multiplier.
		 *\~french
		 *\brief		Les UBOs pour la géométrie, le sous-maillage et la passe donnés.
		 *\param[in]	geometry		La géometrie.
		 *\param[in]	submesh			Le submesh.
		 *\param[in]	pass			La passe du matériau.
		 *\param[in]	instanceMult	Le multiplicateur d'instance.
		 */
		C3D_API PoolsEntry getUbos( Geometry const & geometry
			, Submesh const & submesh
			, Pass const & pass
			, uint32_t instanceMult )const;
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Vide la collection.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void clear( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Adds an object.
		 *\param[in]	element	The object.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	element	L'objet.
		 */
		C3D_API void add( ElementPtr element );
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	name	The object name.
		 *\param[in]	parent	The parent scene node.
		 *\param[in]	mesh	The mesh.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	name	Le nom d'objet.
		 *\param[in]	parent	Le noeud de scène parent.
		 *\param[in]	mesh	Le maillage.
		 *\return		L'objet créé.
		 */
		C3D_API ElementPtr add( Key const & name
			, SceneNode & parent
			, MeshSPtr mesh );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	name	Le nom d'objet.
		 */
		C3D_API void remove( Key const & name );

	private:
		void doCreateEntry( RenderDevice const & device
			, Geometry const & geometry
			, Submesh const & submesh
			, Pass const & pass );
		void doRemoveEntry( RenderDevice const & device
			, Geometry const & geometry
			, Submesh const & submesh
			, Pass const & pass );
		void doRegister( Geometry & geometry );
		void doUnregister( Geometry & geometry );

	private:
		uint32_t m_faceCount{ 0 };
		uint32_t m_vertexCount{ 0 };
		std::map< size_t, PoolsEntry > m_baseEntries;
		std::map< size_t, PoolsEntry > m_entries;
		std::map< Geometry *, OnSubmeshMaterialChangedConnection > m_connections;
		using RenderPassSet = std::set< SceneRenderPass const * >;
		std::map< uint32_t, RenderPassSet > m_instances;
	};
}

#endif
