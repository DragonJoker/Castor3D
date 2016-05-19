#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"

namespace Castor3D
{
	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	inline ManagerView< ResourceType, ManagerType, EventType >::ManagerView( Castor::String const & p_name, ManagerType & p_manager )
		: Castor::Named( p_name )
		, m_manager( p_manager )
	{
	}

	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	inline ManagerView< ResourceType, ManagerType, EventType >::~ManagerView()
	{
	}

	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	template< typename ... Params >
	inline std::shared_ptr< ResourceType > ManagerView< ResourceType, ManagerType, EventType >::Create( Castor::String const & p_name, Params && ... p_params )
	{
		std::shared_ptr< ResourceType > l_resource = m_manager.Create( p_name, std::forward< Params >( p_params )... );

		if ( l_resource )
		{
			m_createdElements.insert( p_name );
		}

		return l_resource;
	}

	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	void ManagerView< ResourceType, ManagerType, EventType >::Insert( Castor::String const & p_name, std::shared_ptr< ResourceType > p_element )
	{
		m_createdElements.insert( p_name );
	}

	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	inline void ManagerView< ResourceType, ManagerType, EventType >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_manager.Find( l_name );

			if ( l_resource )
			{
				m_manager.Remove( l_name );
				m_manager.GetEngine()->PostEvent( MakeFunctorEvent( EventType, [l_resource]()
				{
					l_resource->Cleanup();
				} ) );
			}
		}
	}

	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	inline bool ManagerView< ResourceType, ManagerType, EventType >::Has( Castor::String const & p_name )const
	{
		return m_manager.Has( p_name );
	}

	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	inline std::shared_ptr< ResourceType > ManagerView< ResourceType, ManagerType, EventType >::Find( Castor::String const & p_name )const
	{
		return m_manager.Find( p_name );
	}

	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	inline void ManagerView< ResourceType, ManagerType, EventType >::Remove( Castor::String const & p_name )
	{
		return m_manager.Remove( p_name );
	}
}
