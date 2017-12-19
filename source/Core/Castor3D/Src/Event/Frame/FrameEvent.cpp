#include "FrameEvent.hpp"

#include <Miscellaneous/Debug.hpp>

using namespace castor;

namespace castor3d
{
	FrameEvent::FrameEvent( EventType p_type )
		: m_type( p_type )
	{
#if !defined( NDEBUG )

		StringStream stream;
		stream << Debug::Backtrace{ 20 };
		m_stackTrace = stream.str();

#endif
	}

	FrameEvent::~FrameEvent()
	{
	}
}
