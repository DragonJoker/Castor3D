#include "Castor3D/Cache/CacheView.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

namespace castor3d
{
	template< typename CacheT, EventType EventT >
	inline CacheViewT< CacheT, EventT >::CacheViewT( castor::String const & name
		, CacheT & cache
		, ElementInitialiserT initialise
		, ElementCleanerT clean )
		: castor::Named{ name }
		, m_cache{ cache }
		, m_initialise{ std::move( initialise ) }
		, m_clean{ std::move( clean ) }
	{
	}

	template< typename CacheT, EventType EventT >
	inline CacheViewT< CacheT, EventT >::~CacheViewT()
	{
	}

	template< typename CacheT, EventType EventT >
	inline void CacheViewT< CacheT, EventT >::clear()
	{
		auto lock( castor::makeUniqueLock( m_cache ) );

		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.tryRemove( name );

			if ( resource )
			{
				m_cleaning.push_back( resource );

				if ( m_clean )
				{
					m_clean( resource );
				}
			}
		}
	}

	template< typename CacheT, EventType EventT >
	template< typename ... ParametersT >
	inline typename CacheViewT< CacheT, EventT >::ElementPtrT CacheViewT< CacheT, EventT >::add( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name
		, ParametersT && ... params )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		ElementPtrT created;
		auto result = m_cache.tryAdd( name
			, true
			, created
			, std::forward< ParametersT >( params )... );

		if ( result == created )
		{
			if ( m_initialise )
			{
				m_initialise( result );
			}

			m_createdElements.insert( name );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline bool CacheViewT< CacheT, EventT >::tryAdd( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name
		, typename CacheViewT< CacheT, EventT >::ElementPtrT element
		, bool initialise )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		auto result = m_cache.tryAdd( name, element, initialise );

		if ( result == element )
		{
			if ( initialise && m_initialise )
			{
				m_initialise( element );
			}

			m_createdElements.insert( name );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementPtrT CacheViewT< CacheT, EventT >::add( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name
		, typename CacheViewT< CacheT, EventT >::ElementPtrT element
		, bool initialise )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		auto result = m_cache.add( name, element, initialise );

		if ( result == element )
		{
			if ( initialise && m_initialise )
			{
				m_initialise( element );
			}

			m_createdElements.insert( name );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline bool CacheViewT< CacheT, EventT >::isEmpty()const
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		return m_createdElements.empty();
	}

	template< typename CacheT, EventType EventT >
	inline bool CacheViewT< CacheT, EventT >::has( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name )const
	{
		return m_createdElements.end() != m_createdElements.find( name );
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementPtrT CacheViewT< CacheT, EventT >::tryFind( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name )const
	{
		auto it = m_createdElements.find( name );
		return it != m_createdElements.end()
			? m_cache.tryFind( name )
			: nullptr;
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementPtrT CacheViewT< CacheT, EventT >::find( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name )const
	{
		auto result = tryFind( name );

		if ( !result )
		{
			m_cache.reportUnknown( name );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementPtrT CacheViewT< CacheT, EventT >::tryRemove( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		ElementPtrT result;
		auto it = m_createdElements.find( name );

		if ( it != m_createdElements.end() )
		{
			result = m_cache.tryRemove( name );
			m_createdElements.erase( it );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline void CacheViewT< CacheT, EventT >::remove( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name )
	{
		if ( !tryRemove( name ) )
		{
			m_cache.reportUnknown( name );
		}
	}
}
