#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

namespace castor
{
	namespace Debug
	{
#if defined( NDEBUG )

		template< typename CharT >
		inline void doShowBacktrace( std::basic_ostream< CharT > & stream, int, int )
		{
		}

#endif
	}
}
