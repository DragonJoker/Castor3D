#include "Engine.hpp"
#include "FunctorEvent.hpp"

namespace Castor3D
{
	template< typename ResourceType, typename ManagerType >
	inline ManagerView< ResourceType, ManagerType >::ManagerView( Castor::String const & p_name, ManagerType & p_manager )
		: Castor::Named( p_name )
		, m_manager( p_manager )
	{
	}

	template< typename ResourceType, typename ManagerType >
	inline ManagerView< ResourceType, ManagerType >::~ManagerView()
	{
	}

	template< typename ResourceType, typename ManagerType >
	template< typename ... Params >
	inline std::shared_ptr< ResourceType > ManagerView< ResourceType, ManagerType >::Create( Castor::String const & p_name, Params && ... p_params )
	{
		std::shared_ptr< ResourceType > l_resource = m_manager.Create( p_name, std::forward< Params >( p_params )... );

		if ( l_resource )
		{
			m_createdElements.insert( p_name );
		}

		return l_resource;
	}

	template< typename ResourceType, typename ManagerType >
	void ManagerView< ResourceType, ManagerType >::Insert( Castor::String const & p_name, std::shared_ptr< ResourceType > p_element )
	{
		m_createdElements.insert( p_name );
	}

	template< typename ResourceType, typename ManagerType >
	inline void ManagerView< ResourceType, ManagerType >::Clear()
	{
		for ( auto l_name : m_createdElements )
		{
			auto l_resource = m_manager.Find( l_name );

			if ( l_resource )
			{
				m_manager.Remove( l_name );
				m_manager.GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [l_resource]()
				{
					l_resource->Cleanup();
				} ) );
			}
		}
	}
}
