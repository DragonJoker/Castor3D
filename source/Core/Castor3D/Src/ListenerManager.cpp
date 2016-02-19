#include "ListenerManager.hpp"

using namespace Castor;

//*************************************************************************************************

namespace Castor3D
{
	const String ManagedObjectNamer< FrameListener >::Name = cuT( "FrameListener" );

	ListenerManager::ListenerManager( Engine & p_engine )
		: ResourceManager< String, FrameListener >( p_engine )
	{
		m_defaultListener = ResourceManager< String, FrameListener >::Create( cuT( "Default" ) );
	}

	ListenerManager::~ListenerManager()
	{
	}

	void ListenerManager::PostEvent( FrameEventSPtr p_pEvent )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		FrameListenerSPtr l_listener = m_defaultListener.lock();

		if ( l_listener )
		{
			l_listener->PostEvent( p_pEvent );
		}
	}

	void ListenerManager::FireEvents( eEVENT_TYPE p_type )
	{
		std::unique_lock< Collection > l_lock( m_elements );

		for ( auto l_pair : m_elements )
		{
			l_pair.second->FireEvents( p_type );
		}
	}
}
