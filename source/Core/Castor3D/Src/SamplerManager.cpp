#include "SamplerManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< Sampler >::Name = cuT( "Sampler" );

	void testSamplerCache( Engine & p_engine )
	{
		auto cache = MakeCache< Sampler, Castor::String >( EngineGetter{ p_engine }, ElementProducer< Sampler, Castor::String >{ p_engine } );
	}
}
