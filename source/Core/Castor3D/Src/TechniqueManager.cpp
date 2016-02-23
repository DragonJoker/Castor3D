#include "TechniqueManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< RenderTechnique >::Name = cuT( "RenderTechnique" );

	RenderTechniqueManager::RenderTechniqueManager( Engine & p_engine )
		: ResourceManager< Castor::String, RenderTechnique >( p_engine )
	{
	}

	RenderTechniqueManager::~RenderTechniqueManager()
	{
	}

	RenderTechniqueSPtr RenderTechniqueManager::Create( Castor::String const & p_name, RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
	{
		auto l_lock = make_unique_lock( *this );
		auto l_return = m_techniqueFactory.Create( p_name, p_renderTarget, p_renderSystem, p_params );
		Insert( p_name, l_return );
		return l_return;
	}

	void RenderTechniqueManager::Update()
	{
		auto l_lock = make_unique_lock( *this );

		for ( auto l_it : *this )
		{
			l_it.second->Update();
		}
	}
}
