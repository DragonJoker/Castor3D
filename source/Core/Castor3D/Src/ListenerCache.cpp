#include "ListenerCache.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< FrameListener >::Name = cuT( "FrameListener" );

	ListenerCache::ListenerCache( EngineGetter && p_get
								  , Producer && p_produce
								  , Initialiser && p_initialise
								  , Cleaner && p_clean )
		: MyCacheType{ std::move( p_get ), std::move( p_produce ), std::move( p_initialise ), std::move( p_clean ) }
	{
		m_defaultListener = Add( cuT( "Default" ) );
	}

	ListenerCache::~ListenerCache()
	{
	}

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
