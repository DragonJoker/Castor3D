/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_NODE_CACHE_H___
#define ___C3D_SCENE_NODE_CACHE_H___

#include "Scene/SceneNode.hpp"
#include "Cache/ObjectCache.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for SceneNode.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour SceneNode.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< SceneNode, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< SceneNode >( KeyType const & ) >;
		using Merger = std::function< void( ObjectCacheBase< SceneNode, KeyType > const &
											, castor::Collection< SceneNode, KeyType > &
											, std::shared_ptr< SceneNode >
											, SceneNodeSPtr
											, SceneNodeSPtr ) >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		SceneNode cache.
	\~french
	\brief		Cache de SceneNode.
	*/
	template<>
	class ObjectCache< SceneNode, castor::String >
		: public ObjectCacheBase< SceneNode, castor::String >
	{
	public:
		using MyObjectCacheType = ObjectCacheBase< SceneNode, castor::String >;
		using MyObjectCacheTraits = typename MyObjectCacheType::MyObjectCacheTraits;
		using Element = typename MyObjectCacheType::Element;
		using Key = typename MyObjectCacheType::Key;
		using Collection = typename MyObjectCacheType::Collection;
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
		 *\param[in]	producer		The element producer.
		 *\param[in]	initialiser		The element initialiser.
		 *\param[in]	cleaner			The element cleaner.
		 *\param[in]	merger			The element collection merger.
		 *\param[in]	attacher		The element attacher (to a scene node).
		 *\param[in]	detacher		The element detacher (from a scene node).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	scene			La scène.
		 *\param[in]	rootNode		Le noeud racine.
		 *\param[in]	rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	rootObjectNode	Le noeud racine des objets.
		 *\param[in]	producer		Le créateur d'objet.
		 *\param[in]	initialiser		L'initialiseur d'objet.
		 *\param[in]	cleaner			Le nettoyeur d'objet.
		 *\param[in]	merger			Le fusionneur de collection d'objets.
		 *\param[in]	attacher		L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	detacher		Le détacheur d'objet (d'un noeud de scène).
		 */
		C3D_API ObjectCache( Engine & engine
			, Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode
			, Producer && producer
			, Initialiser && initialiser = Initialiser{}
			, Cleaner && cleaner = Cleaner{}
			, Merger && merger = Merger{}
			, Attacher && attacher = Attacher{}
			, Detacher && detacher = Detacher{} )
			: MyObjectCacheType{ engine
				, scene
				, rootNode
				, rootCameraNode
				, rootObjectNode
				, std::move( producer )
				, std::move( initialiser )
				, std::move( cleaner )
				, std::move( merger )
				, std::move( attacher )
				, std::move( detacher ) }
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
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	name	The object name.
		 *\param[in]	parent	The parent scene node.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	name	Le nom d'objet.
		 *\param[in]	parent	Le noeud de scène parent.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline ElementPtr add( Key const & name, SceneNodeSPtr parent = nullptr )
		{
			auto lock = castor::makeUniqueLock( this->m_elements );
			ElementPtr result;

			if ( !this->m_elements.has( name ) )
			{
				result = this->m_produce( name );
				this->m_initialise( result );
				this->m_elements.insert( name, result );
				m_attach( result, parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				doReportCreation( name );
				this->onChanged();
			}
			else
			{
				result = this->m_elements.find( name );
				doReportDuplicate( name );
			}

			return result;
		}
	};
	using SceneNodeCache = ObjectCache< SceneNode, castor::String >;
	DECLARE_SMART_PTR( SceneNodeCache );
}

#endif
