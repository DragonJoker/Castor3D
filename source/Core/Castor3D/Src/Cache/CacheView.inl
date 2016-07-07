#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"

namespace Castor3D
{
	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	inline CacheView< ResourceType, CacheType, EventType >::CacheView( Castor::String const & p_name, CacheType & p_cache )
		: Castor::Named( p_name )
		, m_cache( p_cache )
	{
	}

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	inline CacheView< ResourceType, CacheType, EventType >::~CacheView()
	{
	}

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	template< typename ... Params >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::Add( Castor::String const & p_name, Params && ... p_params )
	{
		std::shared_ptr< ResourceType > l_resource = m_cache.Add( p_name, std::forward< Params >( p_params )... );

		if ( l_resource )
		{
			m_createdElements.insert( p_name );
		}

		return l_resource;
	}

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::Add( Castor::String const & p_name, std::shared_ptr< ResourceType > p_element )
	{
		std::shared_ptr< ResourceType > l_resource = m_cache.Add( p_name, p_element );

		if ( l_resource )
		{
			m_createdElements.insert( p_name );
		}

		return l_resource;
	}

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	inline void CacheView< ResourceType, CacheType, EventType >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_cache.Find( l_name );

			if ( l_resource )
			{
				m_cache.Remove( l_name );
				m_cache.GetEngine()->PostEvent( MakeFunctorEvent( EventType, [l_resource]()
				{
					l_resource->Cleanup();
				} ) );
			}
		}
	}

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	inline bool CacheView< ResourceType, CacheType, EventType >::Has( Castor::String const & p_name )const
	{
		return m_cache.Has( p_name );
	}

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	inline std::shared_ptr< ResourceType > CacheView< ResourceType, CacheType, EventType >::Find( Castor::String const & p_name )const
	{
		return m_cache.Find( p_name );
	}

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
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
