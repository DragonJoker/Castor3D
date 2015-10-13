#include "SamplerManager.hpp"

#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	SamplerManager::SamplerManager( Engine & p_engine )
		: Manager< String, Sampler >( p_engine )
	{
	}

	SamplerManager::~SamplerManager()
	{
	}

	SamplerSPtr SamplerManager::Create( String const & p_name )
	{
		m_elements.lock();
		SamplerSPtr l_pReturn;

		if ( p_name.empty() )
		{
			l_pReturn = m_renderSystem->CreateSampler( p_name );
		}
		else
		{
			l_pReturn = m_elements.find( p_name );

			if ( !l_pReturn )
			{
				l_pReturn = m_renderSystem->CreateSampler( p_name );
				m_elements.insert( p_name, l_pReturn );
				GetOwner()->PostEvent( MakeInitialiseEvent( *l_pReturn ) );
			}
		}

		m_elements.unlock();
		return l_pReturn;
	}
}
