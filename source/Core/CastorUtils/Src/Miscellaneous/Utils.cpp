#include "Miscellaneous/Utils.hpp"

#include <thread>

namespace Castor
{
	namespace System
	{
		void Sleep( uint32_t p_uiTime )
		{
			std::this_thread::sleep_for( Milliseconds( p_uiTime ) );
		}
	}
}
