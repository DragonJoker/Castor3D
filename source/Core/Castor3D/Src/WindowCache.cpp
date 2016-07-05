#include "WindowCache.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< RenderWindow >::Name = cuT( "RenderWindow" );

	void Render( Cache< RenderWindow, Castor::String, WindowProducer > & p_cache, bool p_force )
	{
		auto l_lock = make_unique_lock( p_cache );

		for ( auto l_it : p_cache )
		{
			l_it.second->Render( p_force );
		}
	}
}
