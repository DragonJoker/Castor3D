#include "Castor3D/Event/Frame/FrameEvent.hpp"

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
