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
	inline void CacheViewT< CacheT, EventT >::clear()
	{
		auto lock( castor::makeUniqueLock( m_cache ) );

		for ( auto name : m_createdElements )
		{
			if ( auto resource = m_cache.tryRemove( name, false ) )
			{
				if ( m_clean )
				{
					m_clean( *resource );
				}

				m_cleaning.emplace_back( std::move( resource ) );
			}
		}
	}

	template< typename CacheT, EventType EventT >
	template< typename ... ParametersT >
	inline typename CacheViewT< CacheT, EventT >::ElementObsT CacheViewT< CacheT, EventT >::tryAdd( ElementKeyT const & name
		, bool initialise
		, ElementObsT & created
		, ParametersT && ... params )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		auto result = m_cache.tryAdd( name
			, false
			, created
			, std::forward< ParametersT >( params )... );

		if ( result.lock() == created.lock() )
		{
			if ( m_initialise && initialise )
			{
				m_initialise( *result.lock() );
			}

			m_createdElements.insert( name );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	template< typename ... ParametersT >
	inline typename CacheViewT< CacheT, EventT >::ElementObsT CacheViewT< CacheT, EventT >::add( ElementKeyT const & name
		, ParametersT && ... params )
	{
		ElementObsT created;
		return this->tryAdd( name
			, true
			, created
			, std::forward< ParametersT >( params )... );
	}

	template< typename CacheT, EventType EventT >
	inline bool CacheViewT< CacheT, EventT >::tryAdd( typename CacheViewT< CacheT, EventT >::ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		auto result = m_cache.tryAdd( name
			, element
			, false );

		if ( !element )
		{
			if ( initialise && m_initialise )
			{
				m_initialise( *result.lock() );
			}

			m_createdElements.insert( name );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementObsT CacheViewT< CacheT, EventT >::add( ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		auto result = m_cache.add( name
			, element
			, false );

		if ( !element )
		{
			if ( initialise && m_initialise )
			{
				m_initialise( *result.lock() );
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
	inline bool CacheViewT< CacheT, EventT >::has( ElementKeyT const & name )const
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		return m_createdElements.end() != m_createdElements.find( name );
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementObsT CacheViewT< CacheT, EventT >::tryFind( ElementKeyT const & name )const
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		auto it = m_createdElements.find( name );
		return it != m_createdElements.end()
			? m_cache.tryFind( name )
			: ElementObsT{};
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementObsT CacheViewT< CacheT, EventT >::find( ElementKeyT const & name )const
	{
		auto result = tryFind( name );

		if ( !result.lock() )
		{
			m_cache.reportUnknown( name );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline typename CacheViewT< CacheT, EventT >::ElementObsT CacheViewT< CacheT, EventT >::tryRemove( ElementKeyT const & name )
	{
		auto lock( castor::makeUniqueLock( m_cache ) );
		ElementPtrT result;
		auto it = m_createdElements.find( name );

		if ( it != m_createdElements.end() )
		{
			result = m_cache.tryRemove( name, false );
			m_createdElements.erase( it );
		}

		return result;
	}

	template< typename CacheT, EventType EventT >
	inline void CacheViewT< CacheT, EventT >::remove( ElementKeyT const & name )
	{
		if ( !tryRemove( name ) )
		{
			m_cache.reportUnknown( name );
		}
	}
}
