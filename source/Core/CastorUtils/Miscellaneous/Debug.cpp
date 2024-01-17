#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

namespace castor::debug
{
#if defined( NDEBUG )

	template< typename CharT >
	inline void doShowBacktrace( std::basic_ostream< CharT > &, int, int )
	{
	}

#endif
}
