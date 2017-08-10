#include "Miscellaneous/Utils.hpp"

#include <thread>

namespace castor
{
	namespace System
	{
		void sleep( uint32_t p_uiTime )
		{
			std::this_thread::sleep_for( Milliseconds( p_uiTime ) );
		}
	}
}
