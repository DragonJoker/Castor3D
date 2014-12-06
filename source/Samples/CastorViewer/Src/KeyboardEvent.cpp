#include "KeyboardEvent.hpp"

#include <RenderWindow.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	KeyboardEvent::KeyboardEvent( RenderWindowSPtr p_pWindow, wxFrame * p_pMainFrame )
		:	FrameEvent( eEVENT_TYPE_PRE_RENDER )
		,	m_pWindow( p_pWindow )
		,	m_pMainFrame( p_pMainFrame )
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
			m_pMainFrame->ShowFullScreen( !l_pWindow->IsFullscreen(), wxFULLSCREEN_ALL );
			Logger::LogDebug( cuT( "Main frame switched fullscreen" ) );
			l_pWindow->UpdateFullScreen( !l_pWindow->IsFullscreen() );
			Logger::LogDebug( cuT( "Render window switched fullscreen" ) );
		}

		return true;
	}
}
