#include "KeyboardEvent.hpp"

#include "CastorViewer.hpp"
#include "MainFrame.hpp"

#include <Render/RenderWindow.hpp>
#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

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

	bool KeyboardEvent::apply()
	{
		RenderWindowSPtr pWindow = m_window.lock();

		if ( pWindow )
		{
			pWindow->enableFullScreen( !pWindow->isFullscreen() );
		}

		return true;
	}
}
