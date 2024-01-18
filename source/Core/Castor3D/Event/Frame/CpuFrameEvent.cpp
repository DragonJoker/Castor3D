#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

#define C3D_UseEventsStack 0

namespace castor3d
{
	CpuFrameEvent::CpuFrameEvent( CpuFrameEvent const & rhs )
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG ) && C3D_UseEventsStack
		, m_stackTrace{ rhs.m_stackTrace }
#endif
	{
	}

	CpuFrameEvent::CpuFrameEvent( CpuFrameEvent && rhs )noexcept
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG ) && C3D_UseEventsStack
		, m_stackTrace{ std::move( rhs.m_stackTrace ) }
#endif
	{
		rhs.m_skip = true;
	}

	CpuFrameEvent & CpuFrameEvent::operator=( CpuFrameEvent const & rhs )
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG ) && C3D_UseEventsStack
		m_stackTrace = rhs.m_stackTrace;
#endif

		return *this;
	}

	CpuFrameEvent & CpuFrameEvent::operator=( CpuFrameEvent && rhs )noexcept
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG ) && C3D_UseEventsStack
		m_stackTrace = std::move( rhs.m_stackTrace );
#endif
		rhs.m_skip = true;

		return *this;
	}

	CpuFrameEvent::CpuFrameEvent( CpuEventType type )
		: m_type{ type }
	{
#if !defined( NDEBUG ) && C3D_UseEventsStack

		castor::StringStream stream = castor::makeStringStream();
		stream << castor::Debug::Backtrace{ 20 };
		m_stackTrace = stream.str();

#endif
	}
}
