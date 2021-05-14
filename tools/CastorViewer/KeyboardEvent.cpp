#include "CastorViewer/KeyboardEvent.hpp"

#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"

#include <Castor3D/Render/RenderWindow.hpp>

#include <CastorUtils/Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace CastorViewer
{
	KeyboardEvent::KeyboardEvent( RenderWindowSPtr p_window )
		: CpuFrameEvent( EventType::ePreRender )
		, m_window( p_window )
	{
	}

	KeyboardEvent::~KeyboardEvent()
	{
	}

	void KeyboardEvent::apply()
	{
		RenderWindowSPtr pWindow = m_window.lock();

		if ( pWindow )
		{
			pWindow->enableFullScreen( !pWindow->isFullscreen() );
		}
	}
}
