#include "DepthStencilStateManager.hpp"

#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< DepthStencilState >::Name = cuT( "DepthStencilState" );

	DepthStencilStateManager::DepthStencilStateManager( Engine & p_engine )
		: ResourceManager< String, DepthStencilState >( p_engine )
	{
	}

	DepthStencilStateManager::~DepthStencilStateManager()
	{
	}

	DepthStencilStateSPtr DepthStencilStateManager::Create( String const & p_name )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		DepthStencilStateSPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = m_renderSystem->CreateDepthStencilState();
			m_elements.insert( p_name, l_return );
			GetEngine()->PostEvent( MakeInitialiseEvent( *l_return ) );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
		}

		return l_return;
	}
}
