#include "CastorViewer/KeyboardEvent.hpp"

#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"

#include <Castor3D/Render/RenderWindow.hpp>

#include <CastorUtils/Log/Logger.hpp>

namespace CastorViewer
{
	KeyboardEvent::KeyboardEvent( castor3d::RenderWindow & window )
		: castor3d::CpuFrameEvent( castor3d::EventType::ePreRender )
		, m_window( &window )
	{
	}

	KeyboardEvent::~KeyboardEvent()
	{
	}

	void KeyboardEvent::apply()
	{
		m_window->enableFullScreen( !m_window->isFullscreen() );
	}
}
