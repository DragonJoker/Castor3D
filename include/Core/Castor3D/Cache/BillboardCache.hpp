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
	C3D_API size_t hash( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult );
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
			int32_t id;
			size_t hash;
			BillboardBase const & billboard;
			Pass const & pass;
			UniformBufferOffsetT< ModelIndexUboConfiguration > modelIndexUbo{};
			GpuDataBufferOffset * modelDataUbo{};
			UniformBufferOffsetT< BillboardUboConfiguration > billboardUbo{};
			UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo{};
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

		C3D_API void registerPass( SceneRenderPass const & renderPass );
		C3D_API void unregisterPass( SceneRenderPass const * renderPass
			, uint32_t instanceMult );
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
		 *\brief		Cleans up the UBO pools.
		 *\remarks		Assumes the entry has been previously created.
		 *\param		billboard		The instance.
		 *\param		pass			The instance material pass.
		 *\param		instanceMult	The instance multiplier, to know where to register the nodes.
		 *\~french
		 *\brief		Nettoie les pools d'UBO.
		 *\remarks		Considère que l'entrée a été préalablement créée.
		 *\param		billboard		L'instance.
		 *\param		pass			La passe de matériau de l'instance.
		 *\param		instanceMult	Le multiplicateur d'instances, pour savoir où enregistrer les nodes.
		 */
		C3D_API PoolsEntry getUbos( BillboardBase const & billboard
			, Pass const & pass
			, uint32_t instanceMult )const;
		/**
		 *\~english
		 *\return		The models data buffer.
		 *\~french
		 *\return		Le tampon dde données de modèles.
		 */
		ModelDataBuffer & getModelDataBuffer()const
		{
			CU_Require( m_modelDataBuffer );
			return *m_modelDataBuffer;
		}

	private:
		void doCreateEntry( RenderDevice const & device
			, BillboardBase const & billboard
			, Pass const & pass );
		void doRemoveEntry( RenderDevice const & device
			, BillboardBase const & billboard
			, Pass const & pass );

	private:
		std::map< size_t, PoolsEntry > m_baseEntries;
		std::map< size_t, PoolsEntry > m_entries;
		std::map< BillboardBase *, OnBillboardMaterialChangedConnection > m_connections;
		using RenderPassSet = std::set< SceneRenderPass const * >;
		std::map< uint32_t, RenderPassSet > m_instances;
		castor3d::ModelDataBufferSPtr m_modelDataBuffer;
	};
}

#endif
