/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardCache_H___
#define ___C3D_BillboardCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Cache/BillboardUboPools.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for BillboardList.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour BillboardList.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< BillboardList, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< BillboardList >( KeyType const &, SceneNodeSPtr ) >;
		using Merger = std::function< void( ObjectCacheBase< BillboardList, KeyType > const &
			, castor::Collection< BillboardList, KeyType > &
			, std::shared_ptr< BillboardList >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/*!
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
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
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
			, SceneNodeSPtr parent );
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

	private:
		BillboardUboPools & m_pools;
	};
	using BillboardListCache = ObjectCache< BillboardList, castor::String >;
	CU_DeclareSmartPtr( BillboardListCache );
}

#endif
