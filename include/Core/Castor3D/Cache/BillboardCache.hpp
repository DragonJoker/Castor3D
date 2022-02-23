/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardCache_H___
#define ___C3D_BillboardCache_H___

#include "Castor3D/Cache/ObjectCacheBase.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	C3D_API size_t hash( BillboardBase const & billboard
		, Pass const & pass );
	using BillboardCacheTraits = ObjectCacheTraitsT< BillboardList, castor::String >;
	/**
	\~english
	\brief		BillboardList cache.
	\~french
	\brief		Cache de BillboardList.
	*/
	template<>
	class ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits > final
		: public ObjectCacheBaseT< BillboardList, castor::String, BillboardCacheTraits >
	{
	public:
		struct PoolsEntry
		{
			uint32_t id;
			size_t hash;
			BillboardBase const & billboard;
			Pass const & pass;
		};
		using ElementT = BillboardList;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = BillboardCacheTraits;
		using ElementObjectCacheT = ObjectCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementObjectCacheT::ElementPtrT;
		using ElementObsT = typename ElementObjectCacheT::ElementObsT;
		using ElementContT = typename ElementObjectCacheT::ElementContT;
		using ElementInitialiserT = typename ElementObjectCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementObjectCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementObjectCacheT::ElementMergerT;
		using ElementAttacherT = typename ElementObjectCacheT::ElementAttacherT;
		using ElementDetacherT = typename ElementObjectCacheT::ElementDetacherT;
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
		C3D_API ObjectCacheT( Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ObjectCacheT() = default;
		/**
		 *\~english
		 *\brief		Initialises the cache buffers.
		 *\~french
		 *\brief		Initialise les buffers du cache.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();

		C3D_API void registerElement( BillboardBase & billboard );
		C3D_API void unregisterElement( BillboardBase & billboard );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear( RenderDevice const & device );
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
		 *\return		The UBOs for given billboards and pass.
		 *\remarks		Assumes the entry has been previously created.
		 *\param		billboard		The instance.
		 *\param		pass			The instance material pass.
		 *\~french
		 *\brief		Les UBOs pour les billboards et la passe donnés.
		 *\remarks		Considère que l'entrée a été préalablement créée.
		 *\param		billboard		L'instance.
		 *\param		pass			La passe de matériau de l'instance.
		 */
		C3D_API PoolsEntry getUbos( BillboardBase const & billboard
			, Pass const & pass )const;
		/**
		 *\~english
		 *\return		The pool entry for given ID.
		 *\param[in]	nodeId	The entry ID.
		 *\~french
		 *\brief		L'entrée de pool pour l'ID donné.
		 *\param[in]	nodeId	L'ID de l'entrée.
		 */
		C3D_API PoolsEntry const & getEntry( uint32_t nodeId )const;

		ashes::Buffer< ModelBufferConfiguration > const & getModelBuffer()const
		{
			return *m_nodesData;
		}

		ashes::Buffer< BillboardUboConfiguration > const & getBillboardsBuffer()const
		{
			return *m_billboardsData;
		}

	private:
		void doCreateEntry( RenderDevice const & device
			, BillboardBase & billboard
			, Pass const & pass );
		void doRemoveEntry( RenderDevice const & device
			, BillboardBase & billboard
			, Pass const & pass );

	private:
		RenderDevice const & m_device;
		std::map< size_t, PoolsEntry > m_entries;
		std::map< BillboardBase *, OnBillboardMaterialChangedConnection > m_connections;
		using RenderPassSet = std::set< RenderNodesPass const * >;
		ashes::BufferPtr< ModelBufferConfiguration > m_nodesData;
		ashes::BufferPtr< BillboardUboConfiguration >m_billboardsData;
	};
}

#endif
