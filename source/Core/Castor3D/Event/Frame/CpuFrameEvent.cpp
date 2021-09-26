#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

namespace castor3d
{
	CpuFrameEvent::CpuFrameEvent( CpuFrameEvent const & rhs )
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG )
		, m_stackTrace{ rhs.m_stackTrace }
#endif
	{
	}

	CpuFrameEvent::CpuFrameEvent( CpuFrameEvent && rhs )
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG )
		, m_stackTrace{ std::move( rhs.m_stackTrace ) }
#endif
	{
		rhs.m_skip = true;
	}

	CpuFrameEvent & CpuFrameEvent::operator=( CpuFrameEvent const & rhs )
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG )
		m_stackTrace = rhs.m_stackTrace;
#endif

		return *this;
	}

	CpuFrameEvent & CpuFrameEvent::operator=( CpuFrameEvent && rhs )
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG )
		m_stackTrace = std::move( rhs.m_stackTrace );
#endif
		rhs.m_skip = true;

		return *this;
	}

	CpuFrameEvent::CpuFrameEvent( EventType type )
		: m_type{ type }
	{
#if !defined( NDEBUG )

		castor::StringStream stream = castor::makeStringStream();
		stream << castor::Debug::Backtrace{ 20 };
		m_stackTrace = stream.str();

#endif
	}
}
