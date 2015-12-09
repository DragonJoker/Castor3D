﻿#include "WindowManager.hpp"

#include "RenderSystem.hpp"
#include "RenderWindow.hpp"

using namespace Castor;

namespace Castor3D
{
	WindowManager::WindowManager( Engine & p_engine )
		: Manager< uint32_t, RenderWindow >( p_engine )
	{
	}

	WindowManager::~WindowManager()
	{
	}

	RenderWindowSPtr WindowManager::Create()
	{
		std::unique_lock< Collection > l_lock( m_elements );
		RenderWindowSPtr l_return = m_renderSystem->CreateRenderWindow();

		if ( l_return )
		{
			m_elements.insert( l_return->GetIndex(), l_return );
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
