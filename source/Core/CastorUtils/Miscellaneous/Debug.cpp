#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

namespace c3d::debug
{
#if defined( NDEBUG )

	template< typename CharT >
	inline void doShowBacktrace( std::basic_ostream< CharT > &, int, int )
	{
	}

#endif
}
