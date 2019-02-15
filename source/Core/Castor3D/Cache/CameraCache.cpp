#include "CameraCache.hpp"

using namespace castor;

namespace castor3d
{
	template<> const String ObjectCacheTraits< Camera, String >::Name = cuT( "Camera" );
}
