#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

using namespace castor;

namespace castor3d
{
	GpuFrameEvent::GpuFrameEvent( EventType type )
		: m_type{ type }
	{
#if !defined( NDEBUG )

		castor::StringStream stream = castor::makeStringStream();
		stream << Debug::Backtrace{ 20 };
		m_stackTrace = stream.str();

#endif
	}
}
