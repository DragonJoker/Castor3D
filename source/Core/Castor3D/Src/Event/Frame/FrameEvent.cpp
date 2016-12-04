#include "FrameEvent.hpp"

#include <Miscellaneous/Debug.hpp>

using namespace Castor;

namespace Castor3D
{
	FrameEvent::FrameEvent( EventType p_type )
		: m_type( p_type )
	{
#if !defined( NDEBUG )

		StringStream l_stream;
		l_stream << Debug::Backtrace{ 40 };
		m_stackTrace = l_stream.str();

#endif
	}

	FrameEvent::~FrameEvent()
	{
	}
}
