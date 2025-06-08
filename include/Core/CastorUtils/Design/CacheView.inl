#include "CastorUtils/Design/CacheView.hpp"

namespace castor
{
	template< typename CacheT >
	inline CacheViewT< CacheT >::CacheViewT( castor::String const & name
		, CacheT & cache
		, ElementInitialiserT initialise
		, ElementCleanerT clean )
		: castor::Named{ name }
		, m_cache{ cache }
		, m_initialise{ castor::move( initialise ) }
		, m_clean{ castor::move( clean ) }
	{
	}

	template< typename CacheT >
	inline void CacheViewT< CacheT >::clear()
	{
		auto elemsLock( castor::makeUniqueLock( m_elementsMutex ) );
		auto lock( castor::makeUniqueLock( m_cache ) );

		for ( auto name : m_createdElements )
		{
			if ( auto resource = m_cache.doTryRemoveNoLock( name, false ) )
			{
				if ( m_clean )
				{
					m_clean( *resource );
				}

				m_cleaning.emplace_back( castor::move( resource ) );
			}
		}
	}

	template< typename CacheT >
	template< typename ... ParametersT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::addNew( ElementKeyT const & name
		, ParametersT && ... params )
	{
		CacheViewT< CacheT >::ElementObsT result{};
		{
			auto lock( castor::makeUniqueLock( m_cache ) );
			result = m_cache.doTryFindNoLock( name );

			if ( !result )
			{
				auto created = m_cache.create( name
					, castor::forward< ParametersT >( params )... );
				result = m_cache.doAddNoLock( name, created, false );

				if ( m_initialise )
				{
					m_initialise( *result );
				}
			}
		}
		{
			auto elemsLock( castor::makeUniqueLock( m_elementsMutex ) );
			m_createdElements.insert( name );
		}
		return result;
	}

	template< typename CacheT >
	inline bool CacheViewT< CacheT >::tryAdd( typename CacheViewT< CacheT >::ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		bool result{};
		{
			auto lock( castor::makeUniqueLock( m_cache ) );
			result = m_cache.doTryAddNoLock( name
				, element
				, false );

			if ( initialise && m_initialise && !element )
			{
				m_initialise( *result );
			}
		}
		{
			auto elemsLock( castor::makeUniqueLock( m_elementsMutex ) );
			m_createdElements.insert( name );
		}
		return result;
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::add( ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		CacheViewT< CacheT >::ElementObsT result{};
		{
			auto lock( castor::makeUniqueLock( m_cache ) );
			result = m_cache.doAddNoLock( name
				, element
				, false );

			if ( initialise && m_initialise && !element )
			{
				m_initialise( *result );
			}
		}
		{
			auto elemsLock( castor::makeUniqueLock( m_elementsMutex ) );
			m_createdElements.insert( name );
		}
		return result;
	}

	template< typename CacheT >
	inline bool CacheViewT< CacheT >::isEmpty()const
	{
		auto lock( castor::makeUniqueLock( m_elementsMutex ) );
		return m_createdElements.empty();
	}

	template< typename CacheT >
	inline bool CacheViewT< CacheT >::has( ElementKeyT const & name )const
	{
		auto lock( castor::makeUniqueLock( m_elementsMutex ) );
		return m_createdElements.end() != m_createdElements.find( name );
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::tryFind( ElementKeyT const & name )const
	{
		auto elemsLock( castor::makeUniqueLock( m_elementsMutex ) );
		auto lock( castor::makeUniqueLock( m_cache ) );
		auto it = m_createdElements.find( name );
		return it != m_createdElements.end()
			? m_cache.doTryFindNoLock( name )
			: ElementObsT{};
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::find( ElementKeyT const & name )const
	{
		auto result = tryFind( name );

		if ( ElementCacheTraitsT::isElementObsNull( result ) )
		{
			m_cache.reportUnknown( name );
		}

		return result;
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementPtrT CacheViewT< CacheT >::tryRemove( ElementKeyT const & name )
	{
		auto elemsLock( castor::makeUniqueLock( m_elementsMutex ) );
		ElementPtrT result;
		auto it = m_createdElements.find( name );

		if ( it != m_createdElements.end() )
		{
			{
				auto lock( castor::makeUniqueLock( m_cache ) );
				result = m_cache.doTryRemoveNoLock( name, false );
			}
			m_createdElements.erase( it );
		}

		return result;
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementPtrT CacheViewT< CacheT >::remove( ElementKeyT const & name )
	{
		auto result = tryRemove( name );

		if ( !result )
		{
			m_cache.reportUnknown( name );
		}

		return result;
	}
}
