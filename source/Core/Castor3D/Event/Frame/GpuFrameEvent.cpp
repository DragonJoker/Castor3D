#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

using namespace castor;

namespace castor3d
{
	GpuFrameEvent::GpuFrameEvent( GpuFrameEvent const & rhs )
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG )
		, m_stackTrace{ rhs.m_stackTrace }
#endif
	{
	}

	GpuFrameEvent::GpuFrameEvent( GpuFrameEvent && rhs )
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG )
		, m_stackTrace{ std::move( rhs.m_stackTrace ) }
#endif
	{
		rhs.m_skip = true;
	}

	GpuFrameEvent & GpuFrameEvent::operator=( GpuFrameEvent const & rhs )
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG )
		m_stackTrace = rhs.m_stackTrace;
#endif

		return *this;
	}

	GpuFrameEvent & GpuFrameEvent::operator=( GpuFrameEvent && rhs )
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG )
		m_stackTrace = std::move( rhs.m_stackTrace );
#endif
		rhs.m_skip = true;

		return *this;
	}

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
