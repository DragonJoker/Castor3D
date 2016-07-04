#include "AnimatedObjectGroupManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< AnimatedObjectGroup >::Name = cuT( "AnimatedObjectGroup" );

	void Update( Cache< AnimatedObjectGroup, Castor::String, AnimatedObjectGroupProducer > & p_cache )
	{
		auto l_lock = make_unique_lock( p_cache );

		for ( auto l_pair : p_cache )
		{
			l_pair.second->Update();
		}
	}
}
