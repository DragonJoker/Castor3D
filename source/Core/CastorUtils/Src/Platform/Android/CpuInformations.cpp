#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include "Miscellaneous/CpuInformations.hpp"

#include <android/cpufeatures/cpu-features.h>

namespace castor
{
	namespace Platform
	{
		void callCpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
		{
		}

		uint32_t getCoreCount()
		{
			android_getCpuCount();
		}
	}
}

#endif
