/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryCache_H___
#define ___C3D_GeometryCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <set>

namespace castor3d
{
	using GeometryCacheTraits = ObjectCacheTraitsT< Geometry, castor::String >;

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
	class ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >
		: public ObjectCacheBaseT< Geometry, castor::String, GeometryCacheTraits >
	{
	public:
		struct PoolsEntry
		{
			int32_t id;
			size_t hash;
			Geometry const & geometry;
			Submesh const & submesh;
			Pass const & pass;
			UniformBufferOffsetT< ModelIndexUboConfiguration > modelIndexUbo{};
			GpuDataBufferOffset * modelDataUbo{};
			UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesUbo{};
		};
		using ElementT = Geometry;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = GeometryCacheTraits;
		using ElementObjectCacheT = ObjectCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementObjectCacheT::ElementPtrT;
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

		C3D_API void registerPass( RenderNodesPass const & renderPass );
		C3D_API void unregisterPass( RenderNodesPass const * renderPass
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
		C3D_API void add( ElementPtrT element );
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

	public:
		using ElementObjectCacheT::add;

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
		RenderDevice const & m_device;
		uint32_t m_faceCount{ 0 };
		uint32_t m_vertexCount{ 0 };
		std::map< size_t, PoolsEntry > m_baseEntries;
		std::map< size_t, PoolsEntry > m_entries;
		std::map< Geometry *, OnSubmeshMaterialChangedConnection > m_connections;
		using RenderPassSet = std::set< RenderNodesPass const * >;
		std::map< uint32_t, RenderPassSet > m_instances;
		castor3d::ModelDataBufferUPtr m_modelDataBuffer;
	};
}

#endif
