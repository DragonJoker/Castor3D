#include "ListenerCache.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< FrameListener >::Name = cuT( "FrameListener" );

	void ListenerCache::PostEvent( FrameEventSPtr p_pEvent )
	{
		auto l_lock = make_unique_lock( m_elements );
		FrameListenerSPtr l_listener = m_defaultListener.lock();

		if ( l_listener )
		{
			l_listener->PostEvent( p_pEvent );
		}
	}

	void ListenerCache::FireEvents( eEVENT_TYPE p_type )
	{
		auto l_lock = make_unique_lock( m_elements );

		for ( auto l_pair : m_elements )
		{
			l_pair.second->FireEvents( p_type );
		}
	}
}
