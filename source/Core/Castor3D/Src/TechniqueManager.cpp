#include "TechniqueManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< RenderTechnique >::Name = cuT( "RenderTechnique" );

	RenderTechniqueCache::RenderTechniqueCache( Engine & p_engine )
		: CacheType{ EngineGetter{ p_engine }, RenderTechniqueProducer{} }
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
