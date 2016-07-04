#include "TechniqueCache.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< RenderTechnique >::Name = cuT( "RenderTechnique" );

	RenderTechniqueCache::RenderTechniqueCache( EngineGetter && p_get, RenderTechniqueProducer && p_produce )
		: CacheType{ std::move( p_get ), std::move( p_produce ) }
	{
	}

	RenderTechniqueCache::~RenderTechniqueCache()
	{
	}

	void RenderTechniqueCache::Update()
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_it : *this )
		{
			l_it.second->Update();
		}
	}
}
