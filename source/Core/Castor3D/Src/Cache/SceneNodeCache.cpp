#include "SceneNodeCache.hpp"

#include "Engine.hpp"

using namespace Castor;

namespace Castor3D
{
	template<> const String ObjectCacheTraits< SceneNode, String >::Name = cuT( "SceneNode" );
}
