#include "RasteriserStateCache.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< RasteriserState >::Name = cuT( "RasteriserState" );

	void testRasteriserCache( Engine & p_engine )
	{
		auto cache = MakeCache< RasteriserState, Castor::String >( EngineGetter{ p_engine }, RasteriserStateProducer{ p_engine } );
	}
}
