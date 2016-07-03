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

	RenderTechniqueSPtr RenderTechniqueManager::Create( String const & p_name, String const & p_type, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		auto l_lock = make_unique_lock( *this );
		RenderTechniqueSPtr l_return;

		if ( !Has( p_name ) )
		{
			l_return = m_factory.Create( p_type, p_renderTarget, p_renderSystem, p_params );
			Insert( p_name, l_return );
		}
		else
		{
			l_return = Find( p_name );
		}

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
