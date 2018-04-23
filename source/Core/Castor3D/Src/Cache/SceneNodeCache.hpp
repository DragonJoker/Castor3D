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
		 *\param[in]	p_scene				The scene.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\param[in]	p_produce			The element producer.
		 *\param[in]	p_initialise		The element initialiser.
		 *\param[in]	p_clean				The element cleaner.
		 *\param[in]	p_merge				The element collection merger.
		 *\param[in]	p_attach			The element attacher (to a scene node).
		 *\param[in]	p_detach			The element detacher (from a scene node).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	p_scene				La scène.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 *\param[in]	p_produce			Le créateur d'objet.
		 *\param[in]	p_initialise		L'initialiseur d'objet.
		 *\param[in]	p_clean				Le nettoyeur d'objet.
		 *\param[in]	p_merge				Le fusionneur de collection d'objets.
		 *\param[in]	p_attach			L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	p_detach			Le détacheur d'objet (d'un noeud de scène).
		 */
		C3D_API ObjectCache( Engine & engine
							, Scene & p_scene
							, SceneNodeSPtr p_rootNode
							, SceneNodeSPtr p_rootCameraNode
							, SceneNodeSPtr p_rootObjectNode
							, Producer && p_produce
							, Initialiser && p_initialise = Initialiser{}
							, Cleaner && p_clean = Cleaner{}
							, Merger && p_merge = Merger{}
							, Attacher && p_attach = Attacher{}
							, Detacher && p_detach = Detacher{} )
			: MyObjectCacheType{ engine
								 , p_scene
								 , p_rootNode
								 , p_rootCameraNode
								 , p_rootObjectNode
								 , std::move( p_produce )
								 , std::move( p_initialise )
								 , std::move( p_clean )
								 , std::move( p_merge )
								 , std::move( p_attach )
								 , std::move( p_detach ) }
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
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_parent	The parent scene node.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_parent	Le noeud de scène parent.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline ElementPtr add( Key const & p_name, SceneNodeSPtr p_parent = nullptr )
		{
			auto lock = castor::makeUniqueLock( this->m_elements );
			ElementPtr result;

			if ( !this->m_elements.has( p_name ) )
			{
				result = this->m_produce( p_name );
				this->m_initialise( result );
				this->m_elements.insert( p_name, result );
				m_attach( result, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				castor::Logger::logDebug( castor::makeStringStream() << INFO_CACHE_CREATED_OBJECT << this->getObjectTypeName() << cuT( ": " ) << p_name );
				this->onChanged();
			}
			else
			{
				result = this->m_elements.find( p_name );
				castor::Logger::logWarning( castor::makeStringStream() << WARNING_CACHE_DUPLICATE_OBJECT << this->getObjectTypeName() << cuT( ": " ) << p_name );
			}

			return result;
		}
	};
	using SceneNodeCache = ObjectCache< SceneNode, castor::String >;
	DECLARE_SMART_PTR( SceneNodeCache );
}

#endif
