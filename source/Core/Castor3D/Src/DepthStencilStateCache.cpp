#include "DepthStencilStateCache.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< DepthStencilState >::Name = cuT( "DepthStencilState" );

	void testDepthStencilCache( Engine & p_engine )
	{
		auto cache = MakeCache< DepthStencilState, Castor::String >( EngineGetter{ p_engine }, DepthStencilStateProducer{ p_engine } );
	}
}
