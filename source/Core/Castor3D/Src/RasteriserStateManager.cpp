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
		m_elements.lock();
		RasteriserStateSPtr l_pReturn = m_elements.find( p_name );

		if ( m_renderSystem && !l_pReturn )
		{
			l_pReturn = m_renderSystem->CreateRasteriserState();
			m_elements.insert( p_name, l_pReturn );
			GetOwner()->PostEvent( MakeInitialiseEvent( *l_pReturn ) );
		}

		m_elements.unlock();
		return l_pReturn;
	}
}
