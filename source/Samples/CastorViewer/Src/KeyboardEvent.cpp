#include "KeyboardEvent.hpp"

#include "CastorViewer.hpp"
#include "MainFrame.hpp"

#include <RenderWindow.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	KeyboardEvent::KeyboardEvent( RenderWindowSPtr p_window )
		: FrameEvent( eEVENT_TYPE_PRE_RENDER )
		, m_window( p_window )
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
			l_pWindow->UpdateFullScreen( !l_pWindow->IsFullscreen() );
		}

		return true;
	}
}
