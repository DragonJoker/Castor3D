#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#	include "Miscellaneous/Debug.hpp"

#	include "Miscellaneous/StringUtils.hpp"
#	include "Config/MultiThreadConfig.hpp"

#	include <cstdlib>

namespace Castor
{
	namespace Debug
	{
		template< typename CharT >
		inline void DoShowBacktrace (std::basic_ostream< CharT > & p_stream, int p_toCapture, int p_toSkip)
		{
		}

		std::wostream & operator<<( std::wostream & p_stream, Backtrace const & p_backtrace )
		{
			DoShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}

		std::ostream & operator<<( std::ostream & p_stream, Backtrace const & p_backtrace )
		{
			DoShowBacktrace( p_stream, p_backtrace.m_toCapture, p_backtrace.m_toSkip );
			return p_stream;
		}

	}
}

#endif
