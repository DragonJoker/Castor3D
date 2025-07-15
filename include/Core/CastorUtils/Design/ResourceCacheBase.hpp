/*
See LICENSE file in root folder
*/
#ifndef ___CU_ResourceCacheBase_H___
#define ___CU_ResourceCacheBase_H___

#pragma GCC diagnostic ignored "-Wundefined-var-template"

#include "DesignModule.hpp"
#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Multithreading/MultithreadingModule.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <mutex>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

#include <unordered_map>

namespace c3d
{
	template< typename ResT, typename KeyT, typename TraitsT >
	class ResourceCacheBaseT
	{
		template< typename CacheT >
		friend class CacheViewT;

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

		ResourceCacheBaseT( ResourceCacheBaseT const & ) = delete;
		ResourceCacheBaseT & operator=( ResourceCacheBaseT const & ) = delete;
		ResourceCacheBaseT( ResourceCacheBaseT && )noexcept = delete;
		ResourceCacheBaseT & operator=( ResourceCacheBaseT && )noexcept = delete;

	protected:
		/**
		*name
		*	Construction/Desctruction.
		**/
		/**@{*/
		explicit ResourceCacheBaseT( LoggerInstance & logger
			, ElementInitialiserT initialise = ElementInitialiserT{}
			, ElementCleanerT clean = ElementCleanerT{}
			, ElementMergerT merge = ElementMergerT{} )
			: m_logger{ logger }
			, m_initialise{ c3d::move( initialise ) }
			, m_clean{ c3d::move( clean ) }
			, m_merge{ c3d::move( merge ) }
		{
		}

		virtual ~ResourceCacheBaseT()noexcept = default;
		/**@}*/

	public:
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		void cleanup()noexcept
		{
			auto lock( c3d::makeUniqueLock( *this ) );
			this->doCleanupNoLock();
		}
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		void clear()noexcept
		{
			auto lock( c3d::makeUniqueLock( *this ) );
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
					, c3d::forward< ParametersT >( parameters )... );
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
			auto lock( c3d::makeUniqueLock( *this ) );
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
			auto lock( c3d::makeUniqueLock( *this ) );
			return this->doAddNoLock( name
				, element
				, initialise );
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
		ElementObsT addNew( ElementKeyT const & name
			, ParametersT && ... parameters )
		{
			auto lock( c3d::makeUniqueLock( *this ) );
			return this->doAddNewNoLockT( name
				, c3d::forward< ParametersT >( parameters )... );
		}
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\param[in]	cleanup	\p true if the element needs to be cleaned up.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	cleanup	\p true si l'évènement doit être nettoyé.
		 */
		ElementPtrT tryRemove( ElementKeyT const & name
			, bool cleanup = false )noexcept
		{
			auto lock( c3d::makeUniqueLock( *this ) );
			return this->doTryRemoveNoLock( name, cleanup );
		}
		/**
		 *\~english
		 *\brief		Logging version of tryRemove.
		 *\param[in]	name	The element name.
		 *\param[in]	cleanup	\p true if the element needs to be cleaned up.
		 *\~french
		 *\brief		Version journalisante de tryAdd.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	cleanup	\p true si l'évènement doit être nettoyé.
		 */
		ElementPtrT remove( ElementKeyT const & name
			, bool cleanup = false )noexcept
		{
			auto lock( c3d::makeUniqueLock( *this ) );
			auto result = this->doTryRemoveNoLock( name, cleanup );

			if ( !result )
			{
				this->reportUnknown( name );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Renames a resource.
		 *\param[in]	oldName	The current element name.
		 *\param[in]	newName	The new element name.
		 *\~french
		 *\brief		Renomme une ressource.
		 *\param[in]	oldName	Le nom actuel de l'élément.
		 *\param[in]	newName	Le nouveau nom de l'élément.
		 */
		void rename( ElementKeyT const & oldName
			, ElementKeyT const & newName )
		{
			auto lock( c3d::makeUniqueLock( *this ) );

			if ( auto newIt = m_resources.find( newName );
				newIt != m_resources.end() )
			{
				this->reportDuplicate( newName );
				return;
			}

			auto oldIt = m_resources.find( oldName );

			if ( oldIt == m_resources.end() )
			{
				this->reportUnknown( oldName );
				return;
			}

			ElementPtrT element = c3d::move( oldIt->second );
			m_resources.erase( oldIt );
			element->rename( newName );
			m_resources.emplace( newName, c3d::move( element ) );
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
		ElementObsT tryFind( ElementKeyT const & name )const noexcept
		{
			auto lock( c3d::makeUniqueLock( *this ) );
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
			auto lock( c3d::makeUniqueLock( *this ) );
			auto result = this->doTryFindNoLock( name );

			if ( ElementCacheTraitsT::isElementObsNull( result ) )
			{
				this->reportUnknown( name );
			}

			return result;
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
		ElementObsT findNoLock( ElementKeyT const & name )const
		{
			auto result = this->doTryFindNoLock( name );

			if ( ElementCacheTraitsT::isElementObsNull( result ) )
			{
				this->reportUnknown( name );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Merges this cache's elements to the one given.
		 *\param[out]	destination		The destination cache.
		 *\~french
		 *\brief		Met les éléments de ce cache dans ceux de celui donné.
		 *\param[out]	destination		Le cache de destination.
		 */
		void mergeInto( ElementCacheBaseT & destination )
		{
			auto lock( c3d::makeUniqueLock( *this ) );
			auto lockOther( c3d::makeUniqueLock( destination ) );

			if ( m_merge )
			{
				for ( auto & it : *this )
				{
					m_merge( *this, destination.m_resources, c3d::move( it.second ) );
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
			auto lock( c3d::makeUniqueLock( *this ) );

			for ( auto const & it : *this )
			{
				func( *it.second );
			}
		}

		template< typename FuncType >
		void forEach( FuncType func )
		{
			auto lock( c3d::makeUniqueLock( *this ) );

			for ( auto & element : *this )
			{
				func( *element.second );
			}
		}
		/**
		 *\~english
		 *\name Getters.
		 *\~french
		 *\name Accesseurs.
		 **/
		/**@{*/
		uint32_t getObjectCount()const noexcept
		{
			auto lock( c3d::makeUniqueLock( *this ) );
			return uint32_t( m_resources.size() );
		}

		String const & getObjectTypeName()const noexcept
		{
			return ElementCacheTraitsT::Name;
		}

		bool has( ElementKeyT const & name )const noexcept
		{
			return !ElementCacheTraitsT::isElementObsNull( tryFind( name ) );
		}

		bool hasNoLock( ElementKeyT const & name )const noexcept
		{
			return !ElementCacheTraitsT::isElementObsNull( doTryFindNoLock( name ) );
		}

		bool isEmpty()const noexcept
		{
			auto lock( c3d::makeUniqueLock( *this ) );
			return m_resources.empty();
		}

		bool isEmptyNoLock()const noexcept
		{
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
		}

		void unlock()const noexcept
		{
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
		auto begin()noexcept
		{
			return m_resources.begin();
		}

		auto begin()const noexcept
		{
			return m_resources.begin();
		}

		auto end()noexcept
		{
			return m_resources.end();
		}

		auto end()const noexcept
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
		void doCleanupNoLock()const noexcept
		{
			if ( m_clean )
			{
				for ( auto & it : m_resources )
				{
					m_clean( *it.second );
				}
			}
		}

		void doClearNoLock()noexcept
		{
			m_resources.clear();
		}

		template< typename ... ParametersT >
		ElementPtrT doCreateT( ElementKeyT const & name
			, ParametersT && ... parameters )const
		{
			return ElementCacheTraitsT::makeElement( *static_cast< ElementCacheT const * >( this )
				, name
				, c3d::forward< ParametersT >( parameters )... );
		}

		virtual ElementObsT doTryAddNoLock( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = true )
		{
			auto ires = m_resources.emplace( name, ElementPtrT{} );

			if ( ires.second )
			{
				ires.first->second = c3d::move( element );
				auto & elem = ires.first->second;

				if ( initialise && elem && m_initialise )
				{
					m_initialise( *elem );
				}
			}

			return ElementCacheTraitsT::makeElementObs( ires.first->second );
		}

		virtual ElementObsT doAddNoLock( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = true )
		{
			auto result = ElementCacheTraitsT::makeElementObs( element );

			if ( element )
			{
				result = doTryAddNoLock( name
					, element
					, initialise );

				if ( element != nullptr
					&& !ElementCacheTraitsT::areElementsEqual( result, element ) )
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
		ElementObsT doAddNewNoLockT( ElementKeyT const & name
			, ParametersT && ... parameters )
		{
			auto [it, inserted] = m_resources.emplace( name, ElementPtrT{} );

			if ( inserted )
			{
				it->second = create( name
					, c3d::forward< ParametersT >( parameters )... );

				if ( m_initialise )
				{
					m_initialise( *it->second );
				}
			}
			else
			{
				reportDuplicate( name );
			}

			return ElementCacheTraitsT::makeElementObs( it->second );
		}

		ElementPtrT doTryRemoveNoLock( ElementKeyT const & name
			, bool cleanup = false )noexcept
		{
			ElementPtrT result;

			if ( auto it = m_resources.find( name );
				it != m_resources.end() )
			{
				result = c3d::move( it->second );

				if ( cleanup && m_clean )
				{
					m_clean( *result );
				}

				m_resources.erase( it );
			}

			return result;
		}

		ElementObsT doTryFindNoLock( ElementKeyT const & name )const noexcept
		{
			ElementObsT result{};

			if ( auto it = m_resources.find( name );
				it != m_resources.end() )
			{
				ElementPtrT & element = it->second;
				result = ElementCacheTraitsT::makeElementObs( element );
			}

			return result;
		}

	protected:
		using MutexT = c3d::RecursiveMutex;

		LoggerInstance & m_logger;
		CheckedMutexT< MutexT > m_mutex;
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
