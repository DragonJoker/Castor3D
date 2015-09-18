#include "FrameEvent.hpp"

namespace Castor3D
{
	FrameEvent::FrameEvent( eEVENT_TYPE p_type )
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
			p_object.m_type = eEVENT_TYPE_COUNT;
		}

		return *this;
	}

	FrameEvent::~FrameEvent()
	{
	}
}