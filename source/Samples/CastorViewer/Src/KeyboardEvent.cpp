#include "KeyboardEvent.hpp"

#include "CastorViewer.hpp"
#include "MainFrame.hpp"

#include <RenderWindow.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	KeyboardEvent::KeyboardEvent( RenderWindowSPtr p_pWindow )
		: FrameEvent( eEVENT_TYPE_PRE_RENDER )
		, m_window( p_pWindow )
	{
	}

	KeyboardEvent::~KeyboardEvent()
	{
	}

	bool KeyboardEvent::Apply()
	{
		RenderWindowSPtr l_pWindow = m_window.lock();

		if ( l_pWindow )
		{
			wxGetApp().GetMainFrame()->ToggleFullScreen( !l_pWindow->IsFullscreen() );
			Logger::LogDebug( cuT( "Main frame switched fullscreen" ) );
			l_pWindow->UpdateFullScreen( !l_pWindow->IsFullscreen() );
			Logger::LogDebug( cuT( "Render window switched fullscreen" ) );
		}

		return true;
	}
}
