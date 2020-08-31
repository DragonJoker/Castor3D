/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectCacheBase_H___
#define ___C3D_ObjectCacheBase_H___

#pragma GCC diagnostic ignored "-Wundefined-var-template"

#include "CacheModule.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Design/Collection.hpp>

namespace castor3d
{
	template< typename ElementType, typename KeyType >
	class ObjectCacheBase
	{
	protected:
		using MyObjectCacheType = ObjectCacheBase< ElementType, KeyType >;
		using MyObjectCacheTraits = ObjectCacheTraits< ElementType, KeyType >;
		using Element = ElementType;
		using Key = KeyType;
		using Collection = castor::Collection< Element, Key >;
		using LockType = std::unique_lock< Collection >;
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
		inline ObjectCacheBase( Engine & engine
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
			: m_engine( engine )
			, m_scene( scene )
			, m_rootNode( rootNode )
			, m_rootCameraNode( rootCameraNode )
			, m_rootObjectNode( rootObjectNode )
			, m_produce( std::move( produce ) )
			, m_initialise( std::move( initialise ) )
			, m_clean( std::move( clean ) )
			, m_merge( std::move( merge ) )
			, m_attach( std::move( attach ) )
			, m_detach( std::move(  detach ) )
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
			LockType lock{ castor::makeUniqueLock( m_elements ) };

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
		 *\param[in]	name	The object name.
		 *\param[in]	element	The object.
		 *\return		The added object, or the existing one.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	name	Le nom d'objet.
		 *\param[in]	element	L'objet.
		 *\return		L'objet ajouté, ou celui existant.
		 */
		inline ElementPtr add( Key const & name
			, ElementPtr element )
		{
			ElementPtr result{ element };

			if ( element )
			{
				LockType lock{ castor::makeUniqueLock( m_elements ) };

				if ( m_elements.has( name ) )
				{
					result = m_elements.find( name );
					doReportDuplicate( name );
				}
				else
				{
					m_elements.insert( name, element );
					onChanged();
				}
			}
			else
			{
				doReportNull();
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	name		The object name.
		 *\param[in]	parent		The parent scene node.
		 *\param[in]	parameters	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	name		Le nom d'objet.
		 *\param[in]	parent		Le noeud de scène parent.
		 *\param[in]	parameters	Les autres paramètres de construction.
		 *\return		L'objet créé.
		 */
		template< typename ... Parameters >
		inline ElementPtr add( Key const & name
			, SceneNode & parent
			, Parameters && ... parameters )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };
			ElementPtr result;

			if ( !m_elements.has( name ) )
			{
				result = m_produce( name, parent, std::forward< Parameters >( parameters )... );
				m_initialise( result );
				m_elements.insert( name, result );
				m_attach( result, parent, m_rootNode.lock(), m_rootCameraNode.lock(), m_rootObjectNode.lock() );
				doReportCreation( name );
				onChanged();
			}
			else
			{
				result = m_elements.find( name );
				doReportDuplicate( name );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	name		The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	name		Le nom d'objet.
		 */
		inline void remove( Key const & name )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			if ( m_elements.has( name ) )
			{
				auto element = m_elements.find( name );
				m_detach( element );
				m_elements.erase( name );
				onChanged();
			}
		}
		/**
		 *\~english
		 *\return		Merges this cache's elements to the one given.
		 *\param[out]	destination		The destination cache.
		 *\~french
		 *\return		Met les éléments de ce cache dans ceux de celui donné.
		 *\param[out]	destination		Le cache de destination.
		 */
		inline void mergeInto( MyObjectCacheType & destination )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };
			LockType lockOther{ castor::makeUniqueLock( destination.m_elements ) };

			for ( auto it : m_elements )
			{
				m_merge( *this
					, destination.m_elements
					, it.second
					, destination.m_rootCameraNode.lock()
					, destination.m_rootObjectNode.lock() );
			}

			clear();
			onChanged();
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	func	La fonction.
		 */
		template< typename FuncType >
		inline void forEach( FuncType func )const
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			for ( auto const & element : m_elements )
			{
				func( *element.second );
			}
		}
		/**
		 *\~english
		 *\brief		Applies a function to all the elements of this cache.
		 *\param[in]	func	The function.
		 *\~french
		 *\brief		Applique une fonction à tous les éléments de ce cache.
		 *\param[in]	func	La fonction.
		 */
		template< typename FuncType >
		inline void forEach( FuncType func )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			for ( auto & element : m_elements )
			{
				func( *element.second );
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
		 *\param[in]	name	The object name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	name	Le nom d'objet.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		inline bool has( Key const & name )const
		{
			return m_elements.has( name );
		}
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	name	The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	name	Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline ElementPtr find( Key const & name )const
		{
			return m_elements.find( name );
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

	protected:
		inline void doReportCreation( castor::String const & name )
		{
			log::trace << InfoCacheCreatedObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name << std::endl;
		}

		inline void doReportDuplicate( castor::String const & name )
		{
			log::warn << WarningCacheDuplicateObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name << std::endl;
		}

		inline void doReportNull()
		{
			log::warn << WarningCacheNullObject
				<< getObjectTypeName() << std::endl;
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
}

#endif
