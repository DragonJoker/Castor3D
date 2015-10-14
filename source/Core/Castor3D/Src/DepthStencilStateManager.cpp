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
		m_elements.lock();
		DepthStencilStateSPtr l_pReturn = m_elements.find( p_name );

		if ( !l_pReturn )
		{
			l_pReturn = m_renderSystem->CreateDepthStencilState();
			m_elements.insert( p_name, l_pReturn );
			GetOwner()->PostEvent( MakeInitialiseEvent( *l_pReturn ) );
			Logger::LogInfo( cuT( "DepthStencilStateManager::Create - Created DepthStencilState: " ) + p_name + cuT( "" ) );
		}
		else
		{
			Logger::LogWarning( cuT( "DepthStencilStateManager::Create - Duplicate DepthStencilState: " ) + p_name );
		}

		m_elements.unlock();
		return l_pReturn;
	}
}
