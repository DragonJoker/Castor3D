#include "SceneCache.hpp"

#include "SceneNodeCache.hpp"
#include "GeometryCache.hpp"
#include "BillboardCache.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< Scene >::Name = cuT( "Scene" );
	const String CachedObjectNamer< SceneNode >::Name = cuT( "SceneNode" );
	const String CachedObjectNamer< Geometry >::Name = cuT( "Geometry" );
	const String CachedObjectNamer< BillboardList >::Name = cuT( "BillboardList" );

	void testSceneCache( Engine & p_engine )
	{
		auto cache = MakeCache< Scene, Castor::String >( EngineGetter{ p_engine }, ElementProducer< Scene, Castor::String, Engine & >{} );
	}
}
