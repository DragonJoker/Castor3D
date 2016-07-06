#include "ListenerCache.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< FrameListener >::Name = cuT( "FrameListener" );

	void tesFrameListenerCache( Engine & p_engine )
	{
		MakeCache< FrameListener, String >( p_engine, []( String const & p_name )
		{
			return std::make_shared< FrameListener >( p_name );
		} );
	}
}
