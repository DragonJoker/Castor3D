/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheBase_H___
#define ___C3D_CacheBase_H___

#pragma GCC diagnostic ignored "-Wundefined-var-template"

#include "CacheModule.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Design/Collection.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	template< typename ElementType, typename KeyType >
	class CacheBase
	{
	public:
		using MyCacheType = CacheBase< ElementType, KeyType >;
		using MyCacheTraits = CacheTraits< ElementType, KeyType >;
		using Element = ElementType;
		using Key = KeyType;
		using Collection = castor::Collection< Element, Key >;
		using LockType = std::unique_lock< Collection >;
		using ElementPtr = std::shared_ptr< Element >;
		using Producer = typename MyCacheTraits::Producer;
		using Merger = typename MyCacheTraits::Merger;
		using Initialiser = ElementInitialiser< Element >;
		using Cleaner = ElementCleaner< Element >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	produce		The element producer.
		 *\param[in]	initialise	The element initialiser.
		 *\param[in]	clean		The element cleaner.
		 *\param[in]	merge		The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	produce		Le créateur d'objet.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean		Le nettoyeur d'objet.
		 *\param[in]	merge		Le fusionneur de collection d'objets.
		 */
		CacheBase( Engine & engine
			, Producer && produce
			, Initialiser && initialise
			, Cleaner && clean
			, Merger && merge )
			: m_engine{ engine }
			, m_elements{ castor3d::getLogger( engine ) }
			, m_produce{ std::move( produce ) }
			, m_initialise{ std::move( initialise ) }
			, m_clean{ std::move( clean ) }
			, m_merge{ std::move( merge ) }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~CacheBase()
		{
		}
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		void cleanup()
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			for ( auto it : m_elements )
			{
				m_clean( it.second );
			}
		}
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		void clear()
		{
			m_elements.clear();
		}
		/**
		 *\~english
		 *\return		\p true if the cache is empty.
		 *\~french
		 *\return		\p true si le cache est vide.
		 */
		bool isEmpty()const
		{
			return m_elements.empty();
		}
		/**
		 *\~english
		 *\brief		Creates an element.
		 *\param[in]	name		The element name.
		 *\param[in]	parameters	The other constructor parameters.
		 *\return		The created element.
		 *\~french
		 *\brief		Crée un élément.
		 *\param[in]	name		Le nom d'élément.
		 *\param[in]	parameters	Les autres paramètres de construction.
		 *\return		L'élément créé.
		 */
		template< typename ... Parameters >
		ElementPtr create( Key const & name
			, Parameters && ... parameters )
		{
			this->doReportCreation( name );
			return m_produce( name
				, std::forward< Parameters >( parameters )... );
		}
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	element	L'élément.
		 */
		ElementPtr add( Key const & name, ElementPtr element )
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
		 *\brief		Creates an element.
		 *\param[in]	name		The element name.
		 *\param[in]	parameters	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un élément.
		 *\param[in]	name		Le nom d'élément.
		 *\param[in]	parameters	Les autres paramètres de construction.
		 *\return		L'élément créé.
		 */
		template< typename ... Parameters >
		ElementPtr add( Key const & name, Parameters && ... parameters )
		{
			ElementPtr result;
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			if ( !m_elements.has( name ) )
			{
				result = create( name
					, std::forward< Parameters >( parameters )... );
				m_initialise( result );
				m_elements.insert( name, result );
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
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 */
		void remove( Key const & name )
		{
			m_elements.erase( name );
		}
		/**
		 *\~english
		 *\return		Merges this cache's elements to the one given.
		 *\param[out]	destination		The destination cache.
		 *\~french
		 *\return		Met les éléments de ce cache dans ceux de celui donné.
		 *\param[out]	destination		Le cache de destination.
		 */
		void mergeInto( MyCacheType & destination )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };
			LockType lockOther{ castor::makeUniqueLock( destination.m_elements ) };

			for ( auto it : m_elements )
			{
				m_merge( *this, destination.m_elements, it.second );
			}

			clear();
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
		void forEach( FuncType func )const
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
		void forEach( FuncType func )
		{
			LockType lock{ castor::makeUniqueLock( m_elements ) };

			for ( auto & element : m_elements )
			{
				func( *element.second );
			}
		}
		/**
		 *\~english
		 *\return		The elements count.
		 *\~french
		 *\return		Le nombre d'éléments.
		 */
		uint32_t getObjectCount()const
		{
			return uint32_t( m_elements.size() );
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		Engine * getEngine()const
		{
			return &m_engine;
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		castor::String const & getObjectTypeName()const
		{
			return MyCacheTraits::Name;
		}
		/**
		 *\~english
		 *\param[in]	name	The element name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	name	Le nom d'élément.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		bool has( Key const & name )const
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
		 *\param[in]	name	Le nom d'élément.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		ElementPtr find( Key const & name )const
		{
			return m_elements.find( name );
		}
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		void lock()const
		{
			m_elements.lock();
		}
		/**
		 *\~english
		 *\brief		Unlocks the collection mutex
		 *\~french
		 *\brief		Délocke le mutex de la collection
		 */
		void unlock()const
		{
			m_elements.unlock();
		}
		/**
		 *\~english
		 *\return		The iterator to the beginning of the collection.
		 *\~french
		 *\return		L'itérateur sur le début de la collection.
		 */
		auto begin()
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the beginning of the collection.
		 *\~french
		 *\return		L'itérateur sur le début de la collection.
		 */
		auto begin()const
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the collection.
		 *\~french
		 *\return		L'itérateur sur la fin de la collection.
		 */
		auto end()
		{
			return m_elements.end();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the collection.
		 *\~french
		 *\return		L'itérateur sur la fin de la collection.
		 */
		auto end()const
		{
			return m_elements.end();
		}

	protected:
		void doReportCreation( castor::String const & name )
		{
			log::trace << InfoCacheCreatedObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name << std::endl;
		}

		void doReportDuplicate( castor::String const & name )
		{
			log::warn << WarningCacheDuplicateObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name << std::endl;
		}

		void doReportNull()
		{
			log::warn << WarningCacheNullObject
				<< getObjectTypeName() << std::endl;
		}

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
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
		//!\~english	The elements collection merger.
		//!\~french		Le fusionneur de collection d'éléments.
		Merger m_merge;
	};
}

#endif
