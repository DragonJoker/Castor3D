#include "PrecompiledHeader.h"

#include "main/Module_Main.h"

#include "main/FrameListener.h"

using namespace Castor3D;

void FrameListener :: PostEvent( FrameEventPtr p_event)
{
	if (p_event->GetType() == FrameEvent::ePreRender)
	{
		m_preEvents.push_back( p_event);
	}
	else if (p_event->GetType() == FrameEvent::eQueueRender)
	{
		m_queueEvents.push_back( p_event);
	}
	else if (p_event->GetType() == FrameEvent::ePostRender)
	{
		m_postEvents.push_back( p_event);
	}
}

bool FrameListener :: FireEvents( FrameEvent::eTYPE p_type)
{
	bool l_bReturn = false;

	switch (p_type)
	{
	case FrameEvent::ePreRender:
		l_bReturn = true;

		for (size_t i = 0 ; i < m_preEvents.size() && l_bReturn ; i++)
		{
			l_bReturn = m_preEvents[i]->Apply();
		}

//		vector::deleteAll( m_preEvents);
		m_preEvents.clear();
		break;

	case FrameEvent::eQueueRender:
		l_bReturn = true;

		for (size_t i = 0 ; i < m_queueEvents.size() && l_bReturn ; i++)
		{
			l_bReturn =m_queueEvents[i]->Apply();
		}

//		vector::deleteAll( m_queueEvents);
		m_queueEvents.clear();
		break;

	case FrameEvent::ePostRender:
		l_bReturn = true;

		for (size_t i = 0 ; i < m_postEvents.size() && l_bReturn ; i++)
		{
			l_bReturn = m_postEvents[i]->Apply();
		}

//		vector::deleteAll( m_postEvents);
		m_postEvents.clear();
		break;
	}

	return l_bReturn;
}