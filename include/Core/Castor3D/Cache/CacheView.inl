#include "Castor3D/Cache/CacheView.hpp"

namespace castor3d
{
	template< typename ResourceType, typename CacheType, EventType EventType >
	inline CacheView< ResourceType, CacheType, EventType >::CacheView( castor::String const & name
		, Initialiser && initialise
		, Cleaner && clean
		, CacheType & cache )
		: castor::Named( name )
		, m_cache( cache )
		, m_initialise{ initialise }
		, m_clean{ clean }
	{
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline CacheView< ResourceType, CacheType, EventType >::~CacheView()
	{
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	template< typename ... Params >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::add( castor::String const & name
		, Params && ... params )
	{
		std::shared_ptr< ResourceType > result;

		if ( m_cache.has( name ) )
		{
			result = m_cache.find( name );
		}
		else
		{
			result = m_cache.create( name
				, std::forward< Params >( params )... );
			m_initialise( result );
			m_cache.add( name, result );
			m_createdElements.insert( name );
		}

		return result;
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::add( castor::String const & name
		, std::shared_ptr< ResourceType > element )
	{
		auto result = m_cache.add( name, element );

		if ( result )
		{
			m_createdElements.insert( name );
		}

		return result;
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline void CacheView< ResourceType, CacheType, EventType >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.find( name );

			if ( resource )
			{
				m_cache.remove( name );
				m_cleaning.push_back( resource );
				m_clean( resource );
			}
		}
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline bool CacheView< ResourceType, CacheType, EventType >::isEmpty()const
	{
		return m_createdElements.empty();
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline bool CacheView< ResourceType, CacheType, EventType >::has( castor::String const & p_name )const
	{
		return m_cache.has( p_name );
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::find( castor::String const & p_name )const
	{
		return m_cache.find( p_name );
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline void CacheView< ResourceType, CacheType, EventType >::remove( castor::String const & p_name )
	{
		auto it = m_createdElements.find( p_name );

		if ( it != m_createdElements.end() )
		{
			m_cache.remove( p_name );
			m_createdElements.erase( it );
		}
	}
}
