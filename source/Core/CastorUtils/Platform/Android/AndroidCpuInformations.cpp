#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformAndroid )

#include "CastorUtils/Miscellaneous/CpuInformations.hpp"

#include <android/cpufeatures/cpu-features.h>

namespace castor::platform
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"
	void callCpuid( uint32_t func, Array< int32_t, 4 > & p_data )
	{
	}

	uint32_t getCoreCount()
	{
		return android_getCpuCount();
	}

	String getCPUModel()
	{
		return String{};
	}
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
}

#endif
