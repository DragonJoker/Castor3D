/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryCache_H___
#define ___C3D_GeometryCache_H___

#include "Castor3D/Cache/ObjectCacheBase.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
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
			Geometry const & geometry;
			Submesh const & submesh;
			Pass const & pass;
			UniformBufferOffset< ModelUbo::Configuration > modelUbo;
			UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixUbo;
			UniformBufferOffset< PickingUbo::Configuration > pickingUbo;
			UniformBufferOffset< TexturesUbo::Configuration > texturesUbo;
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
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le nombre d'objets
		 */
		C3D_API void fillInfo( RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Updates the UBO pools data.
		 *\~french
		 *\brief		Met à jour le contenu des pools d'UBO.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Updates the UBO pools in VRAM.
		 *\~french
		 *\brief		Met à jour les pools d'UBO en VRAM.
		 */
		C3D_API void uploadUbos( ashes::CommandBuffer const & commandBuffer )const;
		/**
		 *\~english
		 *\brief		Updates the picking UBO pools in VRAM.
		 *\~french
		 *\brief		Met à jour les pools d'UBO de picking en VRAM.
		 */
		C3D_API void uploadPickingUbos( ashes::CommandBuffer const & commandBuffer )const;
		/**
		 *\~english
		 *\brief		Cleans up the UBO pools.
		 *\~french
		 *\brief		Nettoie les pools d'UBO.
		 */
		C3D_API void cleanupUbos();
		/**
		 *\~english
		 *\return		The UBOs for given geometry, submesh and pass.
		 *\~french
		 *\brief		Les UBOs pour la géométrie, le sous-maillage et la passe donnés.
		 */
		C3D_API PoolsEntry getUbos( Geometry const & geometry
			, Submesh const & submesh
			, Pass const & pass )const;
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
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
		PoolsEntry doCreateEntry( Geometry const & geometry
			, Submesh const & submesh
			, Pass const & pass );
		void doRemoveEntry( Geometry const & geometry
			, Submesh const & submesh
			, Pass const & pass );
		void doRegister( Geometry & geometry );
		void doUnregister( Geometry & geometry );

	private:
		uint32_t m_faceCount{ 0 };
		uint32_t m_vertexCount{ 0 };
		std::map< size_t, PoolsEntry > m_entries;
		std::map< Geometry *, OnSubmeshMaterialChangedConnection > m_connections;
		UniformBufferPool< ModelUbo::Configuration > m_modelUboPool;
		UniformBufferPool< ModelMatrixUbo::Configuration > m_modelMatrixUboPool;
		UniformBufferPool< PickingUbo::Configuration > m_pickingUboPool;
		UniformBufferPool< TexturesUbo::Configuration > m_texturesUboPool;
		RenderPassTimerSPtr m_updateTimer;
		RenderPassTimerSPtr m_updatePickingTimer;
		RenderPassTimerSPtr m_updateTexturesTimer;
	};
}

#endif
