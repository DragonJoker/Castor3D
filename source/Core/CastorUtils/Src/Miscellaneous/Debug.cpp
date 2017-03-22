#include "Debug.hpp"

namespace Castor
{
	namespace Debug
	{
#if defined( NDEBUG )

		template< typename CharT >
		inline void DoShowBacktrace( std::basic_ostream< CharT > & p_stream, int, int )
		{
		}

#endif
	}
}
