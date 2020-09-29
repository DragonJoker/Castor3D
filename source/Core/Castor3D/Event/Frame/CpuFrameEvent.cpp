#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

using namespace castor;

namespace castor3d
{
	CpuFrameEvent::CpuFrameEvent( EventType type )
		: m_type( type )
	{
#if !defined( NDEBUG )

		castor::StringStream stream = castor::makeStringStream();
		stream << Debug::Backtrace{ 20 };
		m_stackTrace = stream.str();

#endif
	}
}
