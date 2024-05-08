#include "CastorUtils/Miscellaneous/Utils.hpp"

#include <thread>

namespace castor::system
{
	void sleep( uint32_t uiTime )
	{
		std::this_thread::sleep_for( Milliseconds( uiTime ) );
	}

	bool isDebug()
	{
#if !defined( NDEBUG )
		return true;
#else
		return false;
#endif
	}
}
