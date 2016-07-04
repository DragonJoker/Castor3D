#include "BlendStateManager.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< BlendState >::Name = cuT( "BlendState" );

	void testBlendCache( Engine & p_engine )
	{
		auto cache = MakeCache< BlendState, Castor::String >( EngineGetter{ p_engine }, ElementProducer< BlendState, Castor::String >{ p_engine } );
	}
}
