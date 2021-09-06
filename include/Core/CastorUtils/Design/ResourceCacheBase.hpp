/*
See LICENSE file in root folder
*/
#ifndef ___CU_ResourceCacheBase_H___
#define ___CU_ResourceCacheBase_H___

#pragma GCC diagnostic ignored "-Wundefined-var-template"

#include "DesignModule.hpp"
#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Log/Logger.hpp"

#include <mutex>
#include <unordered_map>

namespace castor
{
	template< typename ResT, typename KeyT, typename TraitsT >
	class ResourceCacheBaseT
	{
	public:
		using ElementT = ResT;
		using ElementKeyT = KeyT;
		using ElementCacheTraitsT = TraitsT;
		using ElementCacheBaseT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementCacheT = typename ElementCacheTraitsT::ElementCacheT;
		using ElementPtrT = typename ElementCacheTraitsT::ElementPtrT;
		using ElementObsT = typename ElementCacheTraitsT::ElementObsT;
		using ElementContT = typename ElementCacheTraitsT::ElementContT;
		using ElementInitialiserT = typename ElementCacheTraitsT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheTraitsT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheTraitsT::ElementMergerT;

	protected:
		/**
		*name
		*	Construction/Desctruction.
		**/
		/**@{*/
		ResourceCacheBaseT( LoggerInstance & logger
			, ElementInitialiserT initialise = ElementInitialiserT{}
			, ElementCleanerT clean = ElementCleanerT{}
			, ElementMergerT merge = ElementMergerT{} )
			: m_logger{ logger }
			, m_initialise{ std::move( initialise ) }
			, m_clean{ std::move( clean ) }
			, m_merge{ std::move( merge ) }
		{
		}

		~ResourceCacheBaseT() = default;
		/**@}*/

	public:
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		void cleanup()
		{
			auto lock( castor::makeUniqueLock( *this ) );
			this->doCleanupNoLock();
		}
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		void clear()
		{
			auto lock( castor::makeUniqueLock( *this ) );
			this->doClearNoLock();
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
		template< typename ... ParametersT >
		ElementPtrT create( ElementKeyT const & name
			, ParametersT && ... parameters )const
		{
			this->reportCreation( name );
			return this->doCreateT( name
					, std::forward< ParametersT >( parameters )... );
		}
		/**
		 *\~english
		 *\brief			Adds an element.
		 *\param[in]		name		The element name.
		 *\param[in,out]	element		The resource, will be emptied on add (the cache now owns it).
		 *\param[in]		initialise	\p true to initialise the added element (no effect on duplicates).
		 *\return			The real element (added or duplicate original ).
		 *\~french
		 *\brief			Ajoute un élément.
		 *\param[in]		name		Le nom d'élément.
		 *\param[in,out]	element		La ressource, vidée en cas d'ajout (le cache en prend la responsabilité).
		 *\param[in]		initialise	\p true pour initialiser l'élément ajouté (aucun effect sur les doublons).
		 *\return			L'élément réel (ajouté, ou original du doublon).
		 */
		ElementObsT tryAdd( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = false )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doTryAddNoLock( name
				, element
				, initialise );
		}
		/**
		 *\~english
		 *\brief			Logging version of tryAdd.
		 *\param[in]		name		The resource name.
		 *\param[in,out]	element		The resource, will be emptied on add (the cache now owns it).
		 *\param[in]		initialise	\p true to initialise the added element (no effect on duplicates).
		 *\return			The real element (added or duplicate original ).
		 *\~french
		 *\brief			Version journalisante de tryAdd.
		 *\param[in]		name		Le nom de la ressource.
		 *\param[in,out]	element		La ressource, vidée en cas d'ajout (le cache en prend la responsabilité).
		 *\param[in]		initialise	\p true pour initialiser l'élément ajouté (aucun effect sur les doublons).
		 *\return			L'élément réel (ajouté, ou original du doublon).
		 */
		ElementObsT add( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = true )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doAddNoLock( name
				, element
				, initialise );
		}
		/**
		 *\~english
		 *\brief		Add an element, constructed in-place.
		 *\param[in]	name		The element name.
		 *\param[in]	initialise	\p true to initialise the added element (no effect on duplicates).
		 *\param[out]	created		The created element.
		 *\return		The real element (added or duplicate original ).
		 *\~french
		 *\brief		Ajoute un élément, construict sur place.
		 *\param[in]	name		Le nom d'élément.
		 *\param[in]	initialise	\p true pour initialiser l'élément ajouté (aucun effect sur les doublons).
		 *\param[out]	created		L'élément créé.
		 *\return		L'élément réel (ajouté, ou original du doublon).
		 */
		template< typename ... ParametersT >
		ElementObsT tryAdd( ElementKeyT const & name
			, bool initialise
			, ElementObsT & created
			, ParametersT && ... parameters)
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doTryAddNoLockT( name
				, initialise
				, created
				, std::forward< ParametersT >( parameters )... );
		}
		/**
		 *\~english
		 *\brief		Logging version of tryAdd.
		 *\param[in]	name		The element name.
		 *\param[in]	parameters	The other constructor parameters.
		 *\return		The real element (added or duplicate original ).
		 *\~french
		 *\brief		Version journalisante de tryAdd.
		 *\param[in]	name		Le nom d'élément.
		 *\param[in]	parameters	Les autres paramètres de construction.
		 *\return		L'élément réel (ajouté, ou original du doublon).
		 */
		template< typename ... ParametersT >
		ElementObsT add( ElementKeyT const & name
			, ParametersT && ... parameters )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doAddNoLockT( name
				, std::forward< ParametersT >( parameters )... );
		}
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 */
		ElementPtrT tryRemove( ElementKeyT const & name )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doTryRemoveNoLock( name );
		}
		/**
		 *\~english
		 *\brief		Logging version of tryRemove.
		 *\param[in]	name	The element name.
		 *\~french
		 *\brief		Version journalisante de tryAdd.
		 *\param[in]	name	Le nom d'élément.
		 */
		void remove( ElementKeyT const & name )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			auto result = this->doTryRemoveNoLock( name );

			if ( !result )
			{
				this->reportUnknown( name );
			}
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
		ElementObsT tryFind( ElementKeyT const & name )const
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doTryFindNoLock( name );
		}
		/**
		 *\~english
		 *\brief		Logging version of tryRemove.
		 *\param[in]	name	The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Version journalisante de tryAdd.
		 *\param[in]	name	Le nom d'élément.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		ElementObsT find( ElementKeyT const & name )const
		{
			auto lock( castor::makeUniqueLock( *this ) );
			auto result = this->doTryFindNoLock( name );

			if ( !result.lock() )
			{
				this->reportUnknown( name );
			}

			return result;
		}
		/**
		 *\~english
		 *\return		Merges this cache's elements to the one given.
		 *\param[out]	destination		The destination cache.
		 *\~french
		 *\return		Met les éléments de ce cache dans ceux de celui donné.
		 *\param[out]	destination		Le cache de destination.
		 */
		void mergeInto( ElementCacheBaseT & destination )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			auto lockOther( castor::makeUniqueLock( destination ) );

			if ( m_merge )
			{
				for ( auto & it : *this )
				{
					m_merge( *this, destination.m_resources, it.second );
				}
			}

			doClearNoLock();
		}
		/**
		 *\~english
		 *\name Locked loop.
		 *\~french
		 *\name Boucle verrouillée.
		 **/
		template< typename FuncType >
		void forEach( FuncType func )const
		{
			auto lock( castor::makeUniqueLock( *this ) );

			for ( auto const & it : *this )
			{
				func( *it.second );
			}
		}

		template< typename FuncType >
		void forEach( FuncType func )
		{
			auto lock( castor::makeUniqueLock( *this ) );

			for ( auto & element : *this )
			{
				func( *element.second );
			}
		}
		/**@}*/
		/**
		 *\~english
		 *\name Getters.
		 *\~french
		 *\name Accesseurs.
		 **/
		/**@{*/
		uint32_t getObjectCount()const
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return uint32_t( m_resources.size() );
		}

		String const & getObjectTypeName()const
		{
			return ElementCacheTraitsT::Name;
		}

		bool has( ElementKeyT const & name )const
		{
			return tryFind( name ).lock() != ElementPtrT{};
		}

		bool isEmpty()const
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return m_resources.empty();
		}
		/**@}*/
		/**
		*\~english
		*\name Mutex locking.
		*\~french
		*\name Verrouillage de mutex.
		**/
		/**@{*/
		void lock()const
		{
			m_mutex.lock();
			m_locked = true;
		}

		void unlock()const
		{
			m_locked = false;
			m_mutex.unlock();
		}
		/**@}*/
		/**
		*\~english
		*\name Iteration.
		*\~french
		*\name Itération.
		**/
		/**@{*/
		auto begin()
		{
			CU_Require( this->m_locked );
			return m_resources.begin();
		}

		auto begin()const
		{
			CU_Require( this->m_locked );
			return m_resources.begin();
		}

		auto end()
		{
			return m_resources.end();
		}

		auto end()const
		{
			return m_resources.end();
		}
		/**@}*/
		/**
		*\name Logging.
		**/
		/**@{*/
		void reportCreation( String const & name )const
		{
			static const xchar * InfoCacheCreatedObject = cuT( "ResourceCache - Created " );
			m_logger.logTrace( makeStringStream() << InfoCacheCreatedObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name );
		}

		void reportAdded( String const & name )const
		{
			static const xchar * InfoCacheAddedObject = cuT( "ResourceCache - Added " );
			m_logger.logTrace( makeStringStream() << InfoCacheAddedObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name );
		}

		void reportDuplicate( String const & name )const
		{
			static const xchar * WarningCacheDuplicateObject = cuT( "ResourceCache - Duplicate " );
			m_logger.logWarning( makeStringStream() << WarningCacheDuplicateObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name );
		}

		void reportNull()const
		{
			static const xchar * WarningCacheNullObject = cuT( "ResourceCache - Null " );
			m_logger.logWarning( makeStringStream() << WarningCacheNullObject
				<< getObjectTypeName() );
		}

		void reportUnknown( String const & name )const
		{
			static const xchar * WarningCacheUnknownObject = cuT( "ResourceCache - Unknown " );
			m_logger.logWarning( makeStringStream() << WarningCacheUnknownObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name );
		}
		/**@}*/

	protected:
		void doCleanupNoLock()const
		{
			if ( m_clean )
			{
				for ( auto & it : m_resources )
				{
					m_clean( *it.second );
				}
			}
		}

		void doClearNoLock()
		{
			m_resources.clear();
		}

		template< typename ... ParametersT >
		ElementPtrT doCreateT( ElementKeyT const & name
			, ParametersT && ... parameters )const
		{
			return ElementCacheTraitsT::makeElement( *reinterpret_cast< ElementCacheT const * >( this )
				, name
				, std::forward< ParametersT >( parameters )... );
		}

		ElementObsT doTryAddNoLock( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = true )
		{
			auto ires = m_resources.emplace( name, ElementPtrT{} );

			if ( ires.second )
			{
				ires.first->second = std::move( element );
				auto & elem = ires.first->second;

				if ( initialise && elem && m_initialise )
				{
					m_initialise( *elem );
				}
			}

			return ElementObsT{ ires.first->second };
		}

		ElementObsT doAddNoLock( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = true )
		{
			ElementObsT result{ element };

			if ( element )
			{
				result = doTryAddNoLock( name
					, element
					, initialise );

				if ( element != nullptr
					&& element != result.lock() )
				{
					reportDuplicate( name );
				}
				else
				{
					reportAdded( name );
				}
			}
			else
			{
				reportNull();
			}

			return result;
		}

		template< typename ... ParametersT >
		ElementObsT doTryAddNoLockT( ElementKeyT const & name
			, bool initialise
			, ElementObsT & created
			, ParametersT && ... parameters )
		{
			auto ires = m_resources.emplace( name, ElementPtrT{} );

			if ( ires.second )
			{
				ires.first->second = doCreateT( name
					, std::forward< ParametersT >( parameters )... );
				created = ires.first->second;

				if ( initialise && created.lock() && m_initialise )
				{
					m_initialise( *ires.first->second );
				}
			}

			return ires.first->second;
		}

		template< typename ... ParametersT >
		ElementObsT doAddNoLockT( ElementKeyT const & name
			, ParametersT && ... parameters )
		{
			ElementObsT created;
			auto result = doTryAddNoLockT( name
				, true
				, created
				, std::forward< ParametersT >( parameters )... );

			if ( result.lock() != created.lock() )
			{
				reportDuplicate( name );
			}
			else
			{
				reportCreation( name );
			}

			return result;
		}

		ElementPtrT doTryRemoveNoLock( ElementKeyT const & name )
		{
			ElementPtrT result;
			auto it = m_resources.find( name );

			if ( it != m_resources.end() )
			{
				result = std::move( it->second );
				m_resources.erase( it );
			}

			return result;
		}

		ElementObsT doTryFindNoLock( ElementKeyT const & name )const
		{
			ElementObsT result;
			auto it = m_resources.find( name );
			
			if ( it != m_resources.end() )
			{
				ElementPtrT & element = it->second;
				result = ElementObsT{ element };
			}

			return result;
		}

	protected:
		using MutexT = std::recursive_mutex;

		LoggerInstance & m_logger;
		mutable MutexT m_mutex;
		mutable bool m_locked;
		//!\~english	The elements collection.
		//!\~french		La collection d'éléments.
		mutable ElementContT m_resources;
		//!\~english	The element initialiser.
		//!\~french		L'initaliseur d'éléments.
		ElementInitialiserT m_initialise;
		//!\~english	The element cleaner.
		//!\~french		Le nettoyeur d'éléments.
		ElementCleanerT m_clean;
		//!\~english	The elements collection merger.
		//!\~french		Le fusionneur de collection d'éléments.
		ElementMergerT m_merge;
	};
}

#endif
