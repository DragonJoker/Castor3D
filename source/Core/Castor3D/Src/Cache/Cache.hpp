/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_CACHE_H___
#define ___C3D_CACHE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Collection.hpp>
#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>
#include <Log/Logger.hpp>

namespace castor3d
{
	static const xchar * INFO_CACHE_CREATED_OBJECT = cuT( "Cache::create - Created " );
	static const xchar * WARNING_CACHE_DUPLICATE_OBJECT = cuT( "Cache::create - Duplicate " );
	static const xchar * WARNING_CACHE_NULL_OBJECT = cuT( "Cache::Insert - nullptr " );
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for an element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments.
	*/
	template< typename ElementType, typename KeyType >
	class CacheBase
	{
	public:
		using MyCacheType = CacheBase< ElementType, KeyType >;
		using MyCacheTraits = CacheTraits< ElementType, KeyType >;
		using Element = ElementType;
		using Key = KeyType;
		using Collection = castor::Collection< Element, Key >;
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
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	produce		Le créateur d'objet.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 */
		inline CacheBase( Engine & engine
			, Producer && produce
			, Initialiser && initialise
			, Cleaner && clean
			, Merger && merge )
			: m_engine{ engine }
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
		inline ~CacheBase()
		{
		}
		/**
		 *\~english
		 *\brief		sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		inline void cleanup()
		{
			auto lock = castor::makeUniqueLock( m_elements );

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
		inline ElementPtr create( Key const & name
			, Parameters && ... parameters )
		{
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
		inline ElementPtr add( Key const & name, ElementPtr element )
		{
			ElementPtr result{ element };

			if ( element )
			{
				auto lock = castor::makeUniqueLock( m_elements );

				if ( m_elements.has( name ) )
				{
					castor::Logger::logWarning( castor::StringStream()
						<< WARNING_CACHE_DUPLICATE_OBJECT
						<< getObjectTypeName()
						<< cuT( ": " )
						<< name );
					result = m_elements.find( name );
				}
				else
				{
					m_elements.insert( name, element );
				}
			}
			else
			{
				castor::Logger::logWarning( castor::StringStream()
					<< WARNING_CACHE_NULL_OBJECT
					<< getObjectTypeName() );
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
		inline ElementPtr add( Key const & name, Parameters && ... parameters )
		{
			ElementPtr result;
			auto lock = castor::makeUniqueLock( m_elements );

			if ( !m_elements.has( name ) )
			{
				result = m_produce( name
					, std::forward< Parameters >( parameters )... );
				m_initialise( result );
				m_elements.insert( name, result );
				castor::Logger::logDebug( castor::StringStream()
					<< INFO_CACHE_CREATED_OBJECT
					<< getObjectTypeName()
					<< cuT( ": " ) << name );
			}
			else
			{
				result = m_elements.find( name );
				castor::Logger::logWarning( castor::StringStream()
					<< WARNING_CACHE_DUPLICATE_OBJECT
					<< getObjectTypeName()
					<< cuT( ": " )
					<< name );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name		The element name.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name		Le nom d'élément.
		 */
		inline void remove( Key const & name )
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
		inline void mergeInto( MyCacheType & destination )
		{
			auto lock = castor::makeUniqueLock( m_elements );
			auto lockOther = castor::makeUniqueLock( destination.m_elements );

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
		inline void forEach( FuncType func )const
		{
			auto lock = castor::makeUniqueLock( m_elements );

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
			auto lock = castor::makeUniqueLock( m_elements );

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
			return MyCacheTraits::Name;
		}
		/**
		 *\~english
		 *\param[in]	name		The element name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	name		Le nom d'élément.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		inline bool has( Key const & name )const
		{
			return m_elements.has( name );
		}
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	name		The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	name		Le nom d'élément.
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
		 *\return		The iterator to the beginning of the collection.
		 *\~french
		 *\return		L'itérateur sur le début de la collection.
		 */
		inline auto begin()
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the beginning of the collection.
		 *\~french
		 *\return		L'itérateur sur le début de la collection.
		 */
		inline auto begin()const
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the collection.
		 *\~french
		 *\return		L'itérateur sur la fin de la collection.
		 */
		inline auto end()
		{
			return m_elements.end();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the collection.
		 *\~french
		 *\return		L'itérateur sur la fin de la collection.
		 */
		inline auto end()const
		{
			return m_elements.end();
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
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for an element cache.
	\~french
	\brief		Classe de base pour un cache d'éléments.
	*/
	template< typename ElementType, typename KeyType >
	class Cache
		: public CacheBase< ElementType, KeyType >
	{
	public:
		using MyCacheType = CacheBase< ElementType, KeyType >;
		using MyCacheTraits = typename MyCacheType::MyCacheTraits;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	produce		The element producer.
		 *\param[in]	initialise	The element initialiser.
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	produce		Le créateur d'objet.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 */
		inline Cache( Engine & engine
			, Producer && produce
			, Initialiser && initialise = Initialiser{}
			, Cleaner && clean = Cleaner{}
			, Merger && merge = Merger{} )
			: MyCacheType( engine
				, std::move( produce )
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~Cache()
		{
		}
	};
	/**
	 *\~english
	 *\brief		Creates a cache.
	 *\param[in]	engine		The engine.
	 *\param[in]	produce		The element producer.
	 *\param[in]	initialise	The element initialiser.
	 *\param[in]	clean			The element cleaner.
	 *\param[in]	merge			The element collection merger.
	 *\~french
	 *\brief		Crée un cache.
	 *\param[in]	engine		Le moteur.
	 *\param[in]	produce		Le créateur d'objet.
	 *\param[in]	initialise	L'initialiseur d'objet.
	 *\param[in]	clean			Le nettoyeur d'objet.
	 *\param[in]	merge			Le fusionneur de collection d'objets.
	 */
	template< typename ElementType, typename KeyType >
	inline std::unique_ptr< Cache< ElementType, KeyType > >
	makeCache( Engine & engine
		, typename CacheTraits< ElementType, KeyType >::Producer && produce
		, ElementInitialiser< ElementType > && initialise = []( std::shared_ptr< ElementType > ){}
		, ElementCleaner< ElementType > && clean = []( std::shared_ptr< ElementType > ){}
		, typename CacheTraits< ElementType, KeyType >::Merger && merge = []( CacheBase< ElementType, KeyType > const &
			, castor::Collection< ElementType, KeyType > &
			, std::shared_ptr< ElementType > ){} )
	{
		return std::make_unique< Cache< ElementType, KeyType > >( engine
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge ) );
	}
}

#endif
