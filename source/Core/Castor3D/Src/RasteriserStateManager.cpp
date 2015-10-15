#include "RasteriserStateManager.hpp"

#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	RasteriserStateManager::RasteriserStateManager( Engine & p_engine )
		: Manager< String, RasteriserState >( p_engine )
	{
	}

	RasteriserStateManager::~RasteriserStateManager()
	{
	}

	RasteriserStateSPtr RasteriserStateManager::Create( String const & p_name )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		RasteriserStateSPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = m_renderSystem->CreateRasteriserState();
			m_elements.insert( p_name, l_return );
			GetOwner()->PostEvent( MakeInitialiseEvent( *l_return ) );
			Logger::LogInfo( cuT( "RasteriserStateManager::Create - Created RasteriserState: " ) + p_name + cuT( "" ) );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Logger::LogWarning( cuT( "RasteriserStateManager::Create - Duplicate RasteriserState: " ) + p_name );
		}

		return l_return;
	}
}
