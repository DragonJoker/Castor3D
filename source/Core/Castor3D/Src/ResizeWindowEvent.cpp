#include "ResizeWindowEvent.hpp"
#include "RenderTarget.hpp"
#include "RenderWindow.hpp"

using namespace Castor;

namespace Castor3D
{
	ResizeWindowEvent::ResizeWindowEvent( RenderWindow & p_window )
		:	FrameEvent( eEVENT_TYPE_PRE_RENDER )
		,	m_window( p_window )
	{
	}

	ResizeWindowEvent::ResizeWindowEvent( ResizeWindowEvent const & p_copy )
		:	FrameEvent( p_copy )
		,	m_window( p_copy.m_window )
	{
	}

	ResizeWindowEvent::~ResizeWindowEvent()
	{
	}

	ResizeWindowEvent & ResizeWindowEvent::operator=( ResizeWindowEvent const & p_copy )
	{
		ResizeWindowEvent l_evt( p_copy );
		std::swap( this->m_window, l_evt.m_window );
		std::swap( this->m_eType, l_evt.m_eType );
		return *this;
	}

	bool ResizeWindowEvent::Apply()
	{
		RenderTargetSPtr l_pTarget = m_window.GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->Resize();
		}

		return true;
	}
}
