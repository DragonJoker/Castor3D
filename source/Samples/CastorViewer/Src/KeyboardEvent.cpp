#include "KeyboardEvent.hpp"

#include "MainFrame.hpp"

#include <RenderWindow.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	KeyboardEvent::KeyboardEvent( RenderWindowSPtr p_pWindow, MainFrame * p_pMainFrame )
		: FrameEvent( eEVENT_TYPE_PRE_RENDER )
		, m_pWindow( p_pWindow )
		, m_pMainFrame( p_pMainFrame )
	{
	}

	KeyboardEvent::~KeyboardEvent()
	{
	}

	bool KeyboardEvent::Apply()
	{
		RenderWindowSPtr l_pWindow = m_pWindow.lock();

		if ( l_pWindow )
		{
			m_pMainFrame->ToggleFullScreen( !l_pWindow->IsFullscreen() );
			Logger::LogDebug( cuT( "Main frame switched fullscreen" ) );
			l_pWindow->UpdateFullScreen( !l_pWindow->IsFullscreen() );
			Logger::LogDebug( cuT( "Render window switched fullscreen" ) );
		}

		return true;
	}
}
