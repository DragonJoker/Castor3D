/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectCache_H___
#define ___C3D_ObjectCache_H___

#include "Castor3D/Cache/ObjectCacheBase.hpp"

namespace castor3d
{
	template< typename ElementType, typename KeyType >
	class ObjectCache
		: public ObjectCacheBase< ElementType, KeyType >
	{
	protected:
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType >;
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

	public:
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
		inline ObjectCache( Engine & engine
			, Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode
			, Producer && produce
			, Initialiser && initialise = Initialiser{}
			, Cleaner && clean = Cleaner{}
			, Merger && merge = Merger{}
			, Attacher && attach = Attacher{}
			, Detacher && detach = Detacher{} )
			: MyObjectCacheType( engine
				, scene
				, rootNode
				, rootCameraNode
				, rootObjectNode
				, std::move( produce )
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge )
				, std::move( attach )
				, std::move( detach ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~ObjectCache()
		{
		}
	};
	/**
	 *\~english
	 *\brief		Creates an object cache.
	 *\param[in]	engine			The engine.
	 *\param[in]	scene			The scene.
	 *\param[in]	rootNode		The scene root node.
	 *\param[in]	rootCameraNode	The root node for cameras.
	 *\param[in]	rootObjectNode	The root node for objects.
	 *\param[in]	produce			The element producer.
	 *\param[in]	initialise		The element initialiser.
	 *\param[in]	clean			The element cleaner.
	 *\param[in]	merge			The element collection merger.
	 *\param[in]	attach			The element attacher (to a scene node).
	 *\param[in]	detach			The element detacher (from a scene node).
	 *\~french
	 *\brief		Crée un cache d'objets.
	 *\param[in]	engine			Le moteur.
	 *\param[in]	scene			La scène.
	 *\param[in]	rootNode		Le noeud racine de la scène.
	 *\param[in]	rootCameraNode	Le noeud racine pour les caméras.
	 *\param[in]	rootObjectNode	Le noeud racine pour les objets.
	 *\param[in]	produce			Le créateur d'objet.
	 *\param[in]	initialise		L'initialiseur d'objet.
	 *\param[in]	clean			Le nettoyeur d'objet.
	 *\param[in]	merge			Le fusionneur de collection d'objets.
	 *\param[in]	attach			L'attacheur d'objet (à un noeud de scène).
	 *\param[in]	detach			Le détacheur d'objet (d'un noeud de scène).
	 */
	template< typename ElementType, typename KeyType >
	inline std::unique_ptr< ObjectCache< ElementType, KeyType > >
	makeObjectCache( Engine & engine
		, Scene & scene
		, SceneNodeSPtr rootNode
		, SceneNodeSPtr rootCameraNode
		, SceneNodeSPtr rootObjectNode
		, typename ObjectCacheTraits< ElementType, KeyType >::Producer && produce
		, ElementInitialiser< ElementType > && initialise = []( std::shared_ptr< ElementType > ){}
		, ElementCleaner< ElementType > && clean = []( std::shared_ptr< ElementType > ){}
		, typename ObjectCacheTraits< ElementType, KeyType >::Merger && merge = []( ObjectCacheBase< ElementType, KeyType > const &
			, castor::Collection< ElementType, KeyType > &
			, std::shared_ptr< ElementType >
			, SceneNodeSPtr
			, SceneNodeSPtr ){}
		, ElementAttacher< ElementType > && attach = []( std::shared_ptr< ElementType >, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr ){}
		, ElementDetacher< ElementType > && detach = []( std::shared_ptr< ElementType > ){} )
	{
		return std::make_unique< ObjectCache< ElementType, KeyType > >( engine
			, scene
			, rootNode
			, rootCameraNode
			, rootObjectNode
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge )
			, std::move( attach )
			, std::move( detach ) );
	}
}

#endif
