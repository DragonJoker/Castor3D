#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Miscellaneous/CpuInformations.hpp"

#include "Exception/Assertion.hpp"

#include <intrin.h>

namespace Castor
{
#if defined( CASTOR_COMPILER_MSVC )

void call_cpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
{
	__cpuid( p_data.data(), func );
}

#else

	void call_cpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
	{
		uint32_t l_a;
		uint32_t l_b;
		uint32_t l_c;
		uint32_t l_d;
		__get_cpuid( func, &l_a, &l_b, &l_c, &l_d );
		p_data[0] = int32_t( l_a );
		p_data[1] = int32_t( l_b );
		p_data[2] = int32_t( l_c );
		p_data[3] = int32_t( l_d );
	}

#endif

	uint32_t get_core_count()
	{
		SYSTEM_INFO sysinfo = { 0 };
		::GetSystemInfo( &sysinfo );
		return uint32_t( sysinfo.dwNumberOfProcessors );
	}
}

#endif
