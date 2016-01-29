#include "BlendStateManager.hpp"

#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	BlendStateManager::BlendStateManager( Engine & p_engine )
		: Manager< String, BlendState, Engine >( p_engine )
	{
	}

	BlendStateManager::~BlendStateManager()
	{
	}

	BlendStateSPtr BlendStateManager::Create( String const & p_name )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		BlendStateSPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = m_renderSystem->CreateBlendState();
			m_elements.insert( p_name, l_return );
			GetOwner()->PostEvent( MakeInitialiseEvent( *l_return ) );
			Logger::LogInfo( cuT( "BlendStateManager::Create - Created BlendState: " ) + p_name + cuT( "" ) );
		}
		else
		{
			Logger::LogWarning( cuT( "BlendStateManager::Create - Duplicate BlendState: " ) + p_name );
			l_return = m_elements.find( p_name );
		}

		return l_return;
	}
}
