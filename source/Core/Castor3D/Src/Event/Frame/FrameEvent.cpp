#include "FrameEvent.hpp"

#include <Miscellaneous/Debug.hpp>

using namespace castor;

namespace castor3d
{
	FrameEvent::FrameEvent( EventType type )
		: m_type( type )
	{
#if !defined( NDEBUG )

		castor::StringStream stream = castor::makeStringStream();
		stream << Debug::Backtrace{ 20 };
		m_stackTrace = stream.str();

#endif
	}

	FrameEvent::~FrameEvent()
	{
	}
}
