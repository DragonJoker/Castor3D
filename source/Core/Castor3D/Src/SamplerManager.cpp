#include "SamplerManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< Sampler >::Name = cuT( "Sampler" );

	SamplerManager::SamplerManager( Engine & p_engine )
		: ResourceManager< String, Sampler >( p_engine )
	{
	}

	SamplerManager::~SamplerManager()
	{
	}
}
