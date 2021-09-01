/*
See LICENSE file in root folder
*/
#ifndef ___CU_ResourceCacheBase_H___
#define ___CU_ResourceCacheBase_H___

#pragma GCC diagnostic ignored "-Wundefined-var-template"

#include "DesignModule.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Exception/Assertion.hpp"

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
		using ElementPtrT = typename ElementCacheTraitsT::ElementPtrT;
		using ElementContT = typename ElementCacheTraitsT::ElementContT;
		using ElementProducerT = typename ElementCacheTraitsT::ElementProducerT;
		using ElementInitialiserT = typename ElementCacheTraitsT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheTraitsT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheTraitsT::ElementMergerT;

	protected:
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
		ResourceCacheBaseT( LoggerInstance & logger
			, ElementProducerT produce
			, ElementInitialiserT initialise = ElementInitialiserT{}
			, ElementCleanerT clean = ElementCleanerT{}
			, ElementMergerT merge = ElementMergerT{} )
			: m_logger{ logger }
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
		~ResourceCacheBaseT()
		{
		}

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
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\param[in]	element	The element.
		 *\return		\p false if there is already one object at the given key.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	element	L'élément.
		 *\return		\p false si un élément est déjà associé à la clef.
		 */
		ElementPtrT tryAdd( ElementKeyT const & name
			, ElementPtrT element
			, bool initialise = false )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doTryAddNoLock( name, element, initialise );
		}
		/**
		 *\~english
		 *\brief		Adds a resource, given a name.
		 *\param[in]	name		The resource name.
		 *\param[in]	resource	The resource.
		 *\return		The resource at the given name.
		 *\~french
		 *\brief		Ajoute une ressource à partir d'un nom.
		 *\param[in]	name		Le nom de la ressource.
		 *\param[in]	resource	La ressource.
		 *\return		La ressource associée au nom.
		 */
		ElementPtrT add( ElementKeyT const & name
			, ElementPtrT element
			, bool initialise = true )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doAddNoLock( name, element, initialise );
		}
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\param[in]	element	The element.
		 *\return		\p false if there is already one object at the given key.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	element	L'élément.
		 *\return		\p false si un élément est déjà associé à la clef.
		 */
		template< typename ... ParametersT >
		ElementPtrT tryAdd( ElementKeyT const & name
			, bool initialise
			, ElementPtrT & created
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
		 *\brief		Creates an element.
		 *\param[in]	name		The element name.
		 *\param[in]	parameters	The other constructor parameters.
		 *\return		The element at the given name.
		 *\~french
		 *\brief		Crée un élément.
		 *\param[in]	name		Le nom d'élément.
		 *\param[in]	parameters	Les autres paramètres de construction.
		 *\return		L'élément associé au nom.
		 */
		template< typename ... ParametersT >
		ElementPtrT add( ElementKeyT const & name
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
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
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
		ElementPtrT tryFind( ElementKeyT const & name )const
		{
			auto lock( castor::makeUniqueLock( *this ) );
			return this->doTryFindNoLock( name );
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
		ElementPtrT find( ElementKeyT const & name )const
		{
			auto lock( castor::makeUniqueLock( *this ) );
			auto result = this->doTryFindNoLock( name );

			if ( !result )
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
				for ( auto it : *this )
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

		castor::String const & getObjectTypeName()const
		{
			return ElementCacheTraitsT::Name;
		}

		bool has( ElementKeyT const & name )const
		{
			return tryFind( name ) != nullptr;
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
		void reportCreation( castor::String const & name )const
		{
			static const xchar * InfoCacheCreatedObject = cuT( "ResourceCache - Created " );
			m_logger.logTrace( makeStringStream() << InfoCacheCreatedObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name );
		}

		void reportAdded( castor::String const & name )const
		{
			static const xchar * InfoCacheAddedObject = cuT( "ResourceCache - Added " );
			m_logger.logTrace( makeStringStream() << InfoCacheAddedObject
				<< getObjectTypeName()
				<< cuT( ": " )
				<< name );
		}

		void reportDuplicate( castor::String const & name )const
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

		void reportUnknown( castor::String const & name )const
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
				for ( auto it : m_resources )
				{
					m_clean( it.second );
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
			return m_produce
				? m_produce( name
					, std::forward< ParametersT >( parameters )... )
				: nullptr;
		}

		ElementPtrT doTryAddNoLock( ElementKeyT const & name
			, ElementPtrT element
			, bool initialise = true )
		{
			auto ires = m_resources.emplace( name, element );

			if ( ires.second && initialise && element && m_initialise )
			{
				m_initialise( element );
			}

			return ires.first->second;
		}

		ElementPtrT doAddNoLock( ElementKeyT const & name
			, ElementPtrT element
			, bool initialise = true )
		{
			ElementPtrT result{ element };

			if ( result )
			{
				result = doTryAddNoLock( name, result, initialise );

				if ( element != result )
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
		ElementPtrT doTryAddNoLockT( ElementKeyT const & name
			, bool initialise
			, ElementPtrT & created
			, ParametersT && ... parameters )
		{
			auto ires = m_resources.emplace( name, nullptr );

			if ( ires.second )
			{
				ires.first->second = doCreateT( name
					, std::forward< ParametersT >( parameters )... );
				created = ires.first->second;

				if ( initialise && m_initialise )
				{
					m_initialise( ires.first->second );
				}
			}

			return ires.first->second;
		}

		template< typename ... ParametersT >
		ElementPtrT doAddNoLockT( ElementKeyT const & name
			, ParametersT && ... parameters )
		{
			ElementPtrT created;
			auto result = doTryAddNoLockT( name
				, true
				, created
				, std::forward< ParametersT >( parameters )... );

			if ( result != created )
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
			auto element = doTryFindNoLock( name );

			if ( element )
			{
				m_resources.erase( name );
			}

			return element;
		}

		ElementPtrT doTryFindNoLock( ElementKeyT const & name )const
		{
			auto it = m_resources.find( name );
			return it != m_resources.end()
				? it->second
				: nullptr;
		}

	protected:
		using MutexT = std::recursive_mutex;

		LoggerInstance & m_logger;
		mutable MutexT m_mutex;
		mutable bool m_locked;
		//!\~english	The elements collection.
		//!\~french		La collection d'éléments.
		ElementContT m_resources;
		//!\~english	The element producer.
		//!\~french		Le créateur d'éléments.
		ElementProducerT m_produce;
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
