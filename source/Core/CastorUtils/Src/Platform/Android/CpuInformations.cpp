#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include "Miscellaneous/CpuInformations.hpp"

#include <android/cpufeatures/cpu-features.h>

namespace Castor
{
	namespace Platform
	{
		void call_cpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
		{
		}

		uint32_t get_core_count()
		{
			android_getCpuCount();
		}
	}
}

#endif
