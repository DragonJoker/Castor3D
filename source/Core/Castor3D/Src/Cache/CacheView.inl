#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"

namespace Castor3D
{
	template< typename ResourceType, typename CacheType, EventType EventType >
	inline CacheView< ResourceType, CacheType, EventType >::CacheView( Castor::String const & p_name
		, Initialiser && p_initialise
		, Cleaner && p_clean
		, CacheType & p_cache )
		: Castor::Named( p_name )
		, m_cache( p_cache )
		, m_initialise{ p_initialise }
		, m_clean{ p_clean }
	{
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline CacheView< ResourceType, CacheType, EventType >::~CacheView()
	{
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	template< typename ... Params >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::Add( Castor::String const & p_name, Params && ... p_params )
	{
		std::shared_ptr< ResourceType > l_result;

		if ( m_cache.Has( p_name ) )
		{
			l_result = m_cache.Find( p_name );
		}
		else
		{
			l_result = m_cache.Create( p_name, std::forward< Params >( p_params )... );
			m_initialise( l_result );
			m_cache.Add( p_name, l_result );
			m_createdElements.insert( p_name );
		}

		return l_result;
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::Add( Castor::String const & p_name, std::shared_ptr< ResourceType > p_element )
	{
		auto l_result = m_cache.Add( p_name, p_element );

		if ( l_result )
		{
			m_createdElements.insert( p_name );
		}

		return l_result;
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline void CacheView< ResourceType, CacheType, EventType >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_cache.Find( l_name );

			if ( l_resource )
			{
				m_cache.Remove( l_name );
				m_cleaning.push_back( l_resource );
				m_clean( l_resource );
			}
		}
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline bool CacheView< ResourceType, CacheType, EventType >::Has( Castor::String const & p_name )const
	{
		return m_cache.Has( p_name );
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::Find( Castor::String const & p_name )const
	{
		return m_cache.Find( p_name );
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline void CacheView< ResourceType, CacheType, EventType >::Remove( Castor::String const & p_name )
	{
		auto l_it = m_createdElements.find( p_name );

		if ( l_it != m_createdElements.end() )
		{
			m_cache.Remove( p_name );
			m_createdElements.erase( l_it );
		}
	}
}
