/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OBJECT_CACHE_H___
#define ___C3D_OBJECT_CACHE_H___

#include "Cache.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for a scene element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType, typename KeyType >
	class ObjectCacheBase
	{
	protected:
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType >;
		using MyObjectCacheTraits = ObjectCacheTraits< ElementType, KeyType >;
		using Element = ElementType;
		using Key = KeyType;
		using Collection = castor::Collection< Element, Key >;
		using ElementPtr = std::shared_ptr< Element >;
		using Producer = typename MyObjectCacheTraits::Producer;
		using Merger = typename MyObjectCacheTraits::Merger;
		using Initialiser = ElementInitialiser< Element >;
		using Cleaner = ElementCleaner< Element >;
		using Attacher = ElementAttacher< Element >;
		using Detacher = ElementDetacher< Element >;

	public:
		using OnChangedFunction = std::function< void() >;
		using OnChanged = castor::Signal < OnChangedFunction >;

	public:
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
		inline ObjectCacheBase( Engine & engine
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
			: m_engine( engine )
			, m_scene( p_scene )
			, m_rootNode( p_rootNode )
			, m_rootCameraNode( p_rootCameraNode )
			, m_rootObjectNode( p_rootObjectNode )
			, m_produce( std::move( p_produce ) )
			, m_initialise( std::move( p_initialise ) )
			, m_clean( std::move( p_clean ) )
			, m_merge( std::move( p_merge ) )
			, m_attach( std::move( p_attach ) )
			, m_detach( std::move( p_detach ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~ObjectCacheBase()
		{
		}
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		inline void cleanup()
		{
			auto lock = castor::makeUniqueLock( m_elements );

			for ( auto it : m_elements )
			{
				m_detach( it.second );
				m_clean( it.second );
			}

			onChanged();
		}
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		inline void clear()
		{
			m_elements.clear();
		}
		/**
		 *\~english
		 *\return		\p true if the cache is empty.
		 *\~french
		 *\return		\p true si le cache est vide.
		 */
		inline bool isEmpty()const
		{
			return m_elements.empty();
		}
		/**
		 *\~english
		 *\brief		adds an object.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_element	The object.
		 *\return		The added object, or the existing one.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_element	L'objet.
		 *\return		L'objet ajouté, ou celui existant.
		 */
		inline ElementPtr add( Key const & p_name, ElementPtr p_element )
		{
			ElementPtr result{ p_element };

			if ( p_element )
			{
				auto lock = castor::makeUniqueLock( m_elements );

				if ( m_elements.has( p_name ) )
				{
					castor::Logger::logWarning( castor::makeStringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << p_name );
					result = m_elements.find( p_name );
				}
				else
				{
					m_elements.insert( p_name, p_element );
					onChanged();
				}
			}
			else
			{
				castor::Logger::logWarning( castor::makeStringStream() << WARNING_CACHE_NULL_OBJECT << getObjectTypeName() << cuT( ": " ) );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	p_name			The object name.
		 *\param[in]	p_parent		The parent scene node.
		 *\param[in]	p_parameters	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name			Le nom d'objet.
		 *\param[in]	p_parent		Le noeud de scène parent.
		 *\param[in]	p_parameters	Les autres paramètres de construction.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline ElementPtr add( Key const & p_name, SceneNodeSPtr p_parent, Parameters && ... p_parameters )
		{
			auto lock = castor::makeUniqueLock( m_elements );
			ElementPtr result;

			if ( !m_elements.has( p_name ) )
			{
				result = m_produce( p_name, p_parent, std::forward< Parameters >( p_parameters )... );
				m_initialise( result );
				m_elements.insert( p_name, result );
				m_attach( result, p_parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				castor::Logger::logDebug( castor::makeStringStream() << INFO_CACHE_CREATED_OBJECT << getObjectTypeName() << cuT( ": " ) << p_name );
				onChanged();
			}
			else
			{
				result = m_elements.find( p_name );
				castor::Logger::logWarning( castor::makeStringStream() << WARNING_CACHE_DUPLICATE_OBJECT << getObjectTypeName() << cuT( ": " ) << p_name );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 */
		inline void remove( Key const & p_name )
		{
			auto lock = castor::makeUniqueLock( m_elements );

			if ( m_elements.has( p_name ) )
			{
				auto element = m_elements.find( p_name );
				m_detach( element );
				m_elements.erase( p_name );
				onChanged();
			}
		}
		/**
		 *\~english
		 *\return		Merges this cache's elements to the one given.
		 *\param[out]	p_destination		The destination cache.
		 *\~french
		 *\return		Met les éléments de ce cache dans ceux de celui donné.
		 *\param[out]	p_destination		Le cache de destination.
		 */
		inline void mergeInto( MyObjectCacheType & p_destination )
		{
			auto lock = castor::makeUniqueLock( m_elements );
			auto lockOther = castor::makeUniqueLock( p_destination.m_elements );

			for ( auto it : m_elements )
			{
				m_merge( *this, p_destination.m_elements, it.second, p_destination.m_rootCameraNode.lock(), p_destination.m_rootObjectNode.lock() );
			}

			clear();
			onChanged();
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	p_func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	p_func	La fonction.
		 */
		template< typename FuncType >
		inline void forEach( FuncType p_func )const
		{
			auto lock = castor::makeUniqueLock( m_elements );

			for ( auto const & element : m_elements )
			{
				p_func( *element.second );
			}
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	p_func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	p_func	La fonction.
		 */
		template< typename FuncType >
		inline void forEach( FuncType p_func )
		{
			auto lock = castor::makeUniqueLock( m_elements );

			for ( auto & element : m_elements )
			{
				p_func( *element.second );
			}
		}
		/**
		 *\~english
		 *\return		The objects count.
		 *\~french
		 *\return		Le nombre d'objets
		 */
		inline uint32_t getObjectCount()const
		{
			return uint32_t( m_elements.size() );
		}
		/**
		 *\~english
		 *\return		The Engine.
		 *\~french
		 *\return		L'Engine.
		 */
		inline Scene * getScene()const
		{
			return &m_scene;
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline Engine * getEngine()const
		{
			return &m_engine;
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline castor::String const & getObjectTypeName()const
		{
			return MyObjectCacheTraits::Name;
		}
		/**
		 *\~english
		 *\param[in]	p_name		The object name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	p_name		Le nom d'objet.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		inline bool has( Key const & p_name )const
		{
			return m_elements.has( p_name );
		}
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	p_name		The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	p_name		Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline ElementPtr find( Key const & p_name )const
		{
			return m_elements.find( p_name );
		}
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		inline void lock()const
		{
			m_elements.lock();
		}
		/**
		 *\~english
		 *\brief		Unlocks the collection mutex
		 *\~french
		 *\brief		Délocke le mutex de la collection
		 */
		inline void unlock()const
		{
			m_elements.unlock();
		}
		/**
		 *\~english
		 *\brief		Returns an iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto begin()
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the first element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur le premier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto begin()const
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\brief		Returns an iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto end()
		{
			return m_elements.end();
		}
		/**
		 *\~english
		 *\brief		Returns an constant iterator to the after last element of the collection
		 *\return		The iterator
		 *\~french
		 *\brief		Renvoie un itérateur constant sur l'après dernier élément de la collection
		 *\return		L'itérateur
		 */
		inline auto end()const
		{
			return m_elements.end();
		}

	public:
		//!\~english	The signal emitted when the content has changed.
		//!\~french		Le signal émis lorsque le contenu a changé.
		OnChanged onChanged;

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		Scene & m_scene;
		//!\~english	The root node.
		//!\~french		Le noeud père de tous les noeuds de la scène.
		SceneNodeWPtr m_rootNode;
		//!\~english	The root node used only for cameras.
		//!\~french		Le noeud père de tous les noeuds de caméra.
		SceneNodeWPtr m_rootCameraNode;
		//!\~english	The root node for every object other than camera.
		//!\~french		Le noeud père de tous les noeuds d'objet.
		SceneNodeWPtr m_rootObjectNode;
		//!\~english	The elements collection.
		//!\~french		La collection d'éléments.
		mutable Collection m_elements;
		//!\~english	The element producer.
		//!\~french		Le créateur d'éléments.
		Producer m_produce;
		//!\~english	The element initialiser.
		//!\~french		L'initaliseur d'éléments.
		Initialiser m_initialise;
		//!\~english	The element cleaner.
		//!\~french		Le nettoyeur d'éléments.
		Cleaner m_clean;
		//!\~english	The objects collection merger.
		//!\~french		Le fusionneur de collection d'objets.
		Merger m_merge;
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		Attacher m_attach;
		//!\~english	The object detacher.
		//!\~french		Le détacheur d'objet.
		Detacher m_detach;

		friend class Scene;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for a scene element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments de scène.
	*/
	template< typename ElementType, typename KeyType >
	class ObjectCache
		: public ObjectCacheBase< ElementType, KeyType >
	{
	protected:
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType >;
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

	public:
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
		inline ObjectCache( Engine & engine
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
			: MyObjectCacheType( engine
								 , p_scene
								 , p_rootNode
								 , p_rootCameraNode
								 , p_rootObjectNode
								 , std::move( p_produce )
								 , std::move( p_initialise )
								 , std::move( p_clean )
								 , std::move( p_merge )
								 , std::move( p_attach )
								 , std::move( p_detach ) )
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
	 *\param[in]	p_scene				The scene.
	 *\param[in]	p_rootNode			The scene root node.
	 *\param[in]	p_rootCameraNode	The root node for cameras.
	 *\param[in]	p_rootObjectNode	The root node for objects.
	 *\param[in]	p_produce			The element producer.
	 *\param[in]	p_initialise		The element initialiser.
	 *\param[in]	p_clean				The element cleaner.
	 *\param[in]	p_merge				The element collection merger.
	 *\param[in]	p_attach			The element attacher (to a scene node).
	 *\param[in]	p_detach			The element detacher (from a scene node).
	 *\~french
	 *\brief		Crée un cache d'objets.
	 *\param[in]	engine			Le moteur.
	 *\param[in]	p_scene				La scène.
	 *\param[in]	p_rootNode			Le noeud racine de la scène.
	 *\param[in]	p_rootCameraNode	Le noeud racine pour les caméras.
	 *\param[in]	p_rootObjectNode	Le noeud racine pour les objets.
	 *\param[in]	p_produce			Le créateur d'objet.
	 *\param[in]	p_initialise		L'initialiseur d'objet.
	 *\param[in]	p_clean				Le nettoyeur d'objet.
	 *\param[in]	p_merge				Le fusionneur de collection d'objets.
	 *\param[in]	p_attach			L'attacheur d'objet (à un noeud de scène).
	 *\param[in]	p_detach			Le détacheur d'objet (d'un noeud de scène).
	 */
	template< typename ElementType, typename KeyType >
	inline std::unique_ptr< ObjectCache< ElementType, KeyType > >
	makeObjectCache( Engine & engine
		, Scene & p_scene
		, SceneNodeSPtr p_rootNode
		, SceneNodeSPtr p_rootCameraNode
		, SceneNodeSPtr p_rootObjectNode
		, typename ObjectCacheTraits< ElementType, KeyType >::Producer && p_produce
		, ElementInitialiser< ElementType > && p_initialise = []( std::shared_ptr< ElementType > ){}
		, ElementCleaner< ElementType > && p_clean = []( std::shared_ptr< ElementType > ){}
		, typename ObjectCacheTraits< ElementType, KeyType >::Merger && p_merge = []( ObjectCacheBase< ElementType, KeyType > const &
			, castor::Collection< ElementType, KeyType > &
			, std::shared_ptr< ElementType >
			, SceneNodeSPtr
			, SceneNodeSPtr ){}
		, ElementAttacher< ElementType > && p_attach = []( std::shared_ptr< ElementType >, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr ){}
		, ElementDetacher< ElementType > && p_detach = []( std::shared_ptr< ElementType > ){} )
	{
		return std::make_unique< ObjectCache< ElementType, KeyType > >( engine
			, p_scene
			, p_rootNode
			, p_rootCameraNode
			, p_rootObjectNode
			, std::move( p_produce )
			, std::move( p_initialise )
			, std::move( p_clean )
			, std::move( p_merge )
			, std::move( p_attach )
			, std::move( p_detach ) );
	}
}

#endif
