#include "FrameEvent.hpp"

namespace Castor3D
{
	FrameEvent::FrameEvent( eEVENT_TYPE p_eType )
		:	m_eType( p_eType )
	{
	}

	FrameEvent::FrameEvent( FrameEvent const & p_object )
		:	m_eType( p_object.m_eType )
	{
	}

	FrameEvent::FrameEvent( FrameEvent && p_object )
		:	m_eType( std::move( p_object.m_eType ) )
	{
	}

	FrameEvent & FrameEvent::operator =( FrameEvent const & p_object )
	{
		m_eType = p_object.m_eType;
		return *this;
	}

	FrameEvent & FrameEvent::operator =( FrameEvent && p_object )
	{
		if ( this != &p_object )
		{
			m_eType = p_object.m_eType;
			p_object.m_eType = eEVENT_TYPE_COUNT;
		}

		return *this;
	}

	FrameEvent::~FrameEvent()
	{
	}
}