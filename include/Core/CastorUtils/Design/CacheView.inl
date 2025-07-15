#include "CastorUtils/Design/CacheView.hpp"

namespace c3d
{
	template< typename CacheT >
	inline CacheViewT< CacheT >::CacheViewT( c3d::String const & name
		, CacheT & cache
		, ElementInitialiserT initialise
		, ElementCleanerT clean )
		: c3d::Named{ name }
		, m_cache{ cache }
		, m_initialise{ c3d::move( initialise ) }
		, m_clean{ c3d::move( clean ) }
	{
	}

	template< typename CacheT >
	inline void CacheViewT< CacheT >::clear()
	{
		auto elemsLock( c3d::makeUniqueLock( m_elementsMutex ) );
		auto lock( c3d::makeUniqueLock( m_cache ) );

		for ( auto name : m_createdElements )
		{
			if ( auto resource = m_cache.doTryRemoveNoLock( name, false ) )
			{
				if ( m_clean )
				{
					m_clean( *resource );
				}

				m_cleaning.emplace_back( c3d::move( resource ) );
			}
		}
	}

	template< typename CacheT >
	template< typename ... ParametersT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::addNew( ElementKeyT const & name
		, ParametersT && ... params )
	{
		ElementObsT result{};
		{
			auto lock( c3d::makeUniqueLock( m_cache ) );
			result = m_cache.doTryFindNoLock( name );

			if ( !result )
			{
				auto created = m_cache.create( name
					, c3d::forward< ParametersT >( params )... );
				result = m_cache.doAddNoLock( name, created, false );

				if ( m_initialise )
				{
					m_initialise( *result );
				}
			}
		}
		{
			auto elemsLock( c3d::makeUniqueLock( m_elementsMutex ) );
			m_createdElements.insert( name );
		}
		return result;
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::tryAdd( typename CacheViewT< CacheT >::ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		ElementObsT result{};
		{
			auto lock( c3d::makeUniqueLock( m_cache ) );
			result = m_cache.doTryAddNoLock( name
				, element
				, false );

			if ( initialise && m_initialise && !element )
			{
				m_initialise( *result );
			}
		}
		{
			auto elemsLock( c3d::makeUniqueLock( m_elementsMutex ) );
			m_createdElements.insert( name );
		}
		return result;
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::add( ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		ElementObsT result{};
		{
			auto lock( c3d::makeUniqueLock( m_cache ) );
			result = m_cache.doAddNoLock( name
				, element
				, false );

			if ( initialise && m_initialise && !element )
			{
				m_initialise( *result );
			}
		}
		{
			auto elemsLock( c3d::makeUniqueLock( m_elementsMutex ) );
			m_createdElements.insert( name );
		}
		return result;
	}

	template< typename CacheT >
	inline bool CacheViewT< CacheT >::isEmpty()const
	{
		auto lock( c3d::makeUniqueLock( m_elementsMutex ) );
		return m_createdElements.empty();
	}

	template< typename CacheT >
	inline bool CacheViewT< CacheT >::has( ElementKeyT const & name )const
	{
		auto lock( c3d::makeUniqueLock( m_elementsMutex ) );
		return m_createdElements.end() != m_createdElements.find( name );
	}

	template< typename CacheT >
	inline typename CacheViewT< CacheT >::ElementObsT CacheViewT< CacheT >::tryFind( ElementKeyT const & name )const
	{
		auto elemsLock( c3d::makeUniqueLock( m_elementsMutex ) );
		auto lock( c3d::makeUniqueLock( m_cache ) );
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
		auto elemsLock( c3d::makeUniqueLock( m_elementsMutex ) );
		ElementPtrT result;
		auto it = m_createdElements.find( name );

		if ( it != m_createdElements.end() )
		{
			{
				auto lock( c3d::makeUniqueLock( m_cache ) );
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
