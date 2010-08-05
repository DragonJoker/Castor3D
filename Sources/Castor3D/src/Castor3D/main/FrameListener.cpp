#include "PrecompiledHeader.h"

#include "main/Module_Main.h"

#include "main/FrameListener.h"

using namespace Castor3D;

void FrameListener :: PostEvent( FrameEvent * p_event)
{
	if (p_event->GetType() == fetPreRender)
	{
		m_preEvents.push_back( p_event);
	}
	else if (p_event->GetType() == fetQueueRender)
	{
		m_queueEvents.push_back( p_event);
	}
	else if (p_event->GetType() == fetPostRender)
	{
		m_postEvents.push_back( p_event);
	}
}

bool FrameListener :: FireEvents( FrameEventType p_type)
{
	bool l_bReturn = false;

	switch (p_type)
	{
	case fetPreRender:
		l_bReturn = true;

		for (size_t i = 0 ; i < m_preEvents.size() && l_bReturn ; i++)
		{
			l_bReturn = m_preEvents[i]->Apply();
		}

		vector::deleteAll( m_preEvents);
		break;

	case fetQueueRender:
		l_bReturn = true;

		for (size_t i = 0 ; i < m_queueEvents.size() && l_bReturn ; i++)
		{
			l_bReturn =m_queueEvents[i]->Apply();
		}

		vector::deleteAll( m_queueEvents);
		break;

	case fetPostRender:
		l_bReturn = true;

		for (size_t i = 0 ; i < m_postEvents.size() && l_bReturn ; i++)
		{
			l_bReturn = m_postEvents[i]->Apply();
		}

		vector::deleteAll( m_postEvents);
		break;
	}

	return l_bReturn;
}