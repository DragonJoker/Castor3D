/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardUboPools_H___
#define ___C3D_BillboardUboPools_H___

#include "CacheModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	C3D_API size_t hash( BillboardBase const & billboard
		, Pass const & pass );
	C3D_API size_t hash( BillboardBase const & billboard
		, Pass const & pass
		, uint32_t instanceMult );

	class BillboardUboPools
	{
	public:
		struct PoolsEntry
		{
			size_t hash;
			BillboardBase const & billboard;
			Pass const & pass;
			UniformBufferOffsetT< ModelUboConfiguration > modelUbo;
			UniformBufferOffsetT< ModelMatrixUboConfiguration > modelMatrixUbo;
			UniformBufferOffsetT< BillboardUboConfiguration > billboardUbo;
			UniformBufferOffsetT< PickingUboConfiguration > pickingUbo;
			UniformBufferOffsetT< TexturesUboConfiguration > texturesUbo;
			UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param		device	The current device.
		 *\~french
		 *\brief		Constructeur.
		 *\param		device	Le device actuel.
		 */
		C3D_API explicit BillboardUboPools( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Registers a render pass for nodes registration.
		 *\param		renderPass	The render pass.
		 *\~french
		 *\brief		Enregistre une render pass, pour l'enregistrement des nodes.
		 *\param		renderPass	La render pass.
		 */
		C3D_API void registerPass( SceneRenderPass const & renderPass );
		/**
		 *\~english
		 *\brief		Unregisters a render pass.
		 *\param		renderPass		The render pass.
		 *\param		instanceMult	The instance multiplier, to know where the nodes are registered.
		 *\~french
		 *\brief		Désenregistre une render pass.
		 *\param		renderPass		La render pass.
		 *\param		instanceMult	Le multiplicateur d'instances, pour savoir où sont enregistrés les nodes.
		 */
		C3D_API void unregisterPass( SceneRenderPass const * renderPass
			, uint32_t instanceMult );
		/**
		 *\~english
		 *\brief		Updates the UBO pools data.
		 *\~french
		 *\brief		Met à jour le contenu des pools d'UBO.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Register the entries for given billboard.
		 *\param		billboard	The instance.
		 *\~french
		 *\brief		Enregistre les entrées pour le billboard donné.
		 *\param		billboard	L'instance.
		 */
		void registerElement( BillboardBase & billboard );
		/**
		 *\~english
		 *\brief		Removes the entries for given billboard.
		 *\param		billboard	The instance.
		 *\~french
		 *\brief		Supprime les entrées pour le billboard donné.
		 *\param		billboard	L'instance.
		 */
		void unregisterElement( BillboardBase & billboard );
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
		 *\brief		Flushes the pools.
		 *\~french
		 *\brief		Vide les pools.
		 */
		C3D_API void clear();

	private:
		void doCreateEntry( BillboardBase const & billboard
			, Pass const & pass );
		void doRemoveEntry( BillboardBase const & billboard
			, Pass const & pass );

	private:
		RenderDevice const & m_device;
		std::map< size_t, PoolsEntry > m_baseEntries;
		std::map< size_t, PoolsEntry > m_entries;
		std::map< BillboardBase *, OnBillboardMaterialChangedConnection > m_connections;
		RenderPassTimerSPtr m_updateTimer;
		std::shared_ptr< BillboardUboPools > m_billboardPools;
		using RenderPassSet = std::set< SceneRenderPass const * >;
		std::map< uint32_t, RenderPassSet > m_instances;
	};
}

#endif
