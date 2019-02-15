#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FunctorEvent.hpp"

namespace castor3d
{
	template< typename ResourceType, typename CacheType, EventType EventType >
	inline CacheView< ResourceType, CacheType, EventType >::CacheView( castor::String const & p_name
		, Initialiser && p_initialise
		, Cleaner && p_clean
		, CacheType & cache )
		: castor::Named( p_name )
		, m_cache( cache )
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
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::add( castor::String const & p_name, Params && ... p_params )
	{
		std::shared_ptr< ResourceType > result;

		if ( m_cache.has( p_name ) )
		{
			result = m_cache.find( p_name );
		}
		else
		{
			result = m_cache.create( p_name, std::forward< Params >( p_params )... );
			m_initialise( result );
			m_cache.add( p_name, result );
			m_createdElements.insert( p_name );
		}

		return result;
	}

	template< typename ResourceType, typename CacheType, EventType EventType >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::add( castor::String const & p_name, std::shared_ptr< ResourceType > p_element )
	{
		auto result = m_cache.add( p_name, p_element );

		if ( result )
		{
			m_createdElements.insert( p_name );
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
