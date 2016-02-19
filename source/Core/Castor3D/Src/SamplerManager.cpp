#include "SamplerManager.hpp"

#include "RenderSystem.hpp"

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
				Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
			}
			else
			{
				l_return = m_elements.find( p_name );
				Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
			}
		}

		return l_return;
	}
}
