#include "WindowManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< RenderWindow >::Name = cuT( "RenderWindow" );

	WindowManager::WindowManager( Engine & p_engine )
		: ResourceManager< String, RenderWindow >( p_engine )
	{
	}

	WindowManager::~WindowManager()
	{
	}

	RenderWindowSPtr WindowManager::Create( String const & p_name )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		RenderWindowSPtr l_return = std::make_shared< RenderWindow >( *GetEngine(), p_name );

		if ( l_return )
		{
			m_elements.insert( l_return->GetName(), l_return );
		}

		return l_return;
	}

	void WindowManager::Render( bool p_force )
	{
		std::unique_lock< Collection > l_lock( m_elements );

		for ( auto && l_it : m_elements )
		{
			l_it.second->Render( p_force );
		}
	}
}
