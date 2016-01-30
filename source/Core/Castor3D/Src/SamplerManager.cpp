#include "SamplerManager.hpp"

#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	SamplerManager::SamplerManager( Engine & p_engine )
		: ResourceManager< String, Sampler >( p_engine )
	{
	}

	SamplerManager::~SamplerManager()
	{
	}

	SamplerSPtr SamplerManager::Create( String const & p_name )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		SamplerSPtr l_return;

		if ( p_name.empty() )
		{
			l_return = m_renderSystem->CreateSampler( p_name );
		}
		else
		{
			if ( !m_elements.has( p_name ) )
			{
				l_return = m_renderSystem->CreateSampler( p_name );
				m_elements.insert( p_name, l_return );
				GetEngine()->PostEvent( MakeInitialiseEvent( *l_return ) );
				Logger::LogInfo( cuT( "SamplerManager::Create - Created Sampler: " ) + p_name + cuT( "" ) );
			}
			else
			{
				l_return = m_elements.find( p_name );
				Logger::LogWarning( cuT( "SamplerManager::Create - Duplicate Sampler: " ) + p_name );
			}
		}

		return l_return;
	}
}
