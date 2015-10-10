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

	ResizeWindowEvent::~ResizeWindowEvent()
	{
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
