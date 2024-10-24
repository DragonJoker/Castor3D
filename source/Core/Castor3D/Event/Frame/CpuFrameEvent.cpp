#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

namespace castor3d
{
#if !defined( NDEBUG )
	static bool constexpr C3D_UseCpuEventsStack = false;
#endif

	CpuFrameEvent::CpuFrameEvent( CpuFrameEvent const & rhs )
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG )
		, m_stackTrace{ rhs.m_stackTrace }
#endif
	{
	}

	CpuFrameEvent::CpuFrameEvent( CpuFrameEvent && rhs )noexcept
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG )
		, m_stackTrace{ castor::move( rhs.m_stackTrace ) }
#endif
	{
		rhs.m_skip = true;
	}

	CpuFrameEvent & CpuFrameEvent::operator=( CpuFrameEvent const & rhs )
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG )
		if constexpr ( C3D_UseCpuEventsStack )
		{
			m_stackTrace = rhs.m_stackTrace;
		}
#endif

		return *this;
	}

	CpuFrameEvent & CpuFrameEvent::operator=( CpuFrameEvent && rhs )noexcept
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG )
		if constexpr ( C3D_UseCpuEventsStack )
		{
			m_stackTrace = castor::move( rhs.m_stackTrace );
		}
#endif
		rhs.m_skip = true;

		return *this;
	}

	CpuFrameEvent::CpuFrameEvent( CpuEventType type )
		: m_type{ type }
	{
#if !defined( NDEBUG )
		if constexpr ( C3D_UseCpuEventsStack )
		{
			castor::StringStream stream = castor::makeStringStream();
			stream << castor::debug::Backtrace{ 20 };
			m_stackTrace = stream.str();
		}
#endif
	}
}
