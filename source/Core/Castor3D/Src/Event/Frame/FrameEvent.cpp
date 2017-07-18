#include "FrameEvent.hpp"

#include <Miscellaneous/Debug.hpp>

using namespace Castor;

namespace Castor3D
{
	FrameEvent::FrameEvent( EventType p_type )
		: m_type( p_type )
	{
#if !defined( NDEBUG )

		StringStream stream;
		stream << Debug::Backtrace{ 40 };
		m_stackTrace = stream.str();

#endif
	}

	FrameEvent::~FrameEvent()
	{
	}
}
