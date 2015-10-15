#include "DepthStencilStateManager.hpp"

#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	DepthStencilStateManager::DepthStencilStateManager( Engine & p_engine )
		: Manager< String, DepthStencilState >( p_engine )
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
			GetOwner()->PostEvent( MakeInitialiseEvent( *l_return ) );
			Logger::LogInfo( cuT( "DepthStencilStateManager::Create - Created DepthStencilState: " ) + p_name + cuT( "" ) );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Logger::LogWarning( cuT( "DepthStencilStateManager::Create - Duplicate DepthStencilState: " ) + p_name );
		}

		return l_return;
	}
}
