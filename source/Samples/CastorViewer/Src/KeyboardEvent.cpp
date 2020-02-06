#include "KeyboardEvent.hpp"

#include "CastorViewer.hpp"
#include "MainFrame.hpp"

#include <Render/RenderWindow.hpp>
#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	KeyboardEvent::KeyboardEvent( RenderWindowSPtr p_window )
		: FrameEvent( EventType::ePreRender )
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
			l_pWindow->UpdateFullScreen( !l_pWindow->IsFullscreen() );
		}

		return true;
	}
}
