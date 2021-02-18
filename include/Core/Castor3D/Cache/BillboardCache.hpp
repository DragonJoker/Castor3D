/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardCache_H___
#define ___C3D_BillboardCache_H___

#include "Castor3D/Cache/ObjectCacheBase.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Cache/BillboardUboPools.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/04/2018
	\version	0.11.0
	\~english
	\brief		BillboardList cache.
	\~french
	\brief		Cache de BillboardList.
	*/
	template<>
	class ObjectCache< BillboardList, castor::String >
		: public ObjectCacheBase< BillboardList, castor::String >
	{
	public:
		using MyObjectCache = ObjectCacheBase< BillboardList, castor::String >;
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
		/**
		 *\~english
		 *\brief		Initialises the UBO pools.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise les pools d'UBO.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans up the UBO pools.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Nettoie les pools d'UBO.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
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
		 *\brief		Creates a new object and adds it to the collection.
		 *\param[in]	name	The object name.
		 *\param[in]	parent	The parent scene node.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un nouvel objet et l'ajoute à la collection.
		 *\param[in]	name	Le nom d'objet.
		 *\param[in]	parent	Le noeud de scène parent.
		 *\return		L'objet créé.
		 */
		C3D_API ElementPtr add( Key const & name
			, SceneNode & parent );
		/**
		 *\~english
		 *\brief		Adds an existing object.
		 *\param[in]	element	The object.
		 *\~french
		 *\brief		Ajoute un objet existant.
		 *\param[in]	element	L'objet.
		 */
		C3D_API void add( ElementPtr element );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	name	Le nom d'objet.
		 */
		C3D_API void remove( Key const & name );

		BillboardUboPools & getUboPools()const
		{
			return *m_pools;
		}

	private:
		BillboardUboPoolsSPtr m_pools;
		std::set< SceneRenderPass const * > m_pendingPasses;
	};
	using BillboardListCache = ObjectCache< BillboardList, castor::String >;
	CU_DeclareSmartPtr( BillboardListCache );
}

#endif
