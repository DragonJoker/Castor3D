#include "FrameEvent.hpp"

namespace Castor3D
{
	FrameEvent::FrameEvent( EventType p_type )
		:	m_type( p_type )
	{
	}

	FrameEvent::FrameEvent( FrameEvent const & p_object )
		:	m_type( p_object.m_type )
	{
	}

	FrameEvent::FrameEvent( FrameEvent && p_object )
		:	m_type( std::move( p_object.m_type ) )
	{
	}

	FrameEvent & FrameEvent::operator =( FrameEvent const & p_object )
	{
		m_type = p_object.m_type;
		return *this;
	}

	FrameEvent & FrameEvent::operator =( FrameEvent && p_object )
	{
		if ( this != &p_object )
		{
			m_type = p_object.m_type;
			p_object.m_type = EventType::Count;
		}

		return *this;
	}

	FrameEvent::~FrameEvent()
	{
	}
}