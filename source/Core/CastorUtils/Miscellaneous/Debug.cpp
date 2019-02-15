#include "Debug.hpp"

namespace castor
{
	namespace Debug
	{
#if defined( NDEBUG )

		template< typename CharT >
		inline void doShowBacktrace( std::basic_ostream< CharT > & p_stream, int, int )
		{
		}

#endif
	}
}
