#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

namespace castor3d
{
#if !defined( NDEBUG )
	static bool constexpr C3D_UseGpuEventsStack = false;
#endif

	GpuFrameEvent::GpuFrameEvent( GpuFrameEvent const & rhs )
		: m_type{ rhs.m_type }
		, m_skip{ rhs.m_skip.load() }
#if !defined( NDEBUG )
		, m_stackTrace{ rhs.m_stackTrace }
#endif
	{
	}

	GpuFrameEvent::GpuFrameEvent( GpuFrameEvent && rhs )noexcept
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
		if constexpr ( C3D_UseGpuEventsStack )
		{
			m_stackTrace = rhs.m_stackTrace;
		}
#endif

		return *this;
	}

	GpuFrameEvent & GpuFrameEvent::operator=( GpuFrameEvent && rhs )noexcept
	{
		m_type = rhs.m_type;
		m_skip = rhs.m_skip.load();
#if !defined( NDEBUG )
		if constexpr ( C3D_UseGpuEventsStack )
		{
			m_stackTrace = std::move( rhs.m_stackTrace );
		}
#endif
		rhs.m_skip = true;

		return *this;
	}

	GpuFrameEvent::GpuFrameEvent( GpuEventType type )
		: m_type{ type }
	{
#if !defined( NDEBUG )
		if constexpr ( C3D_UseGpuEventsStack )
		{
			castor::StringStream stream = castor::makeStringStream();
			stream << castor::Debug::Backtrace{ 20 };
			m_stackTrace = stream.str();
		}
#endif
	}
}
