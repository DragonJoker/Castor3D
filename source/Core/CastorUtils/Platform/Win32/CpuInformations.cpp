#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/CpuInformations.hpp"

#include <Windows.h>
#include <intrin.h>

namespace castor
{
	namespace Platform
	{
#if defined( CU_CompilerMSVC )

		void callCpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
		{
			__cpuid( p_data.data(), func );
		}

#else

		void callCpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
		{
			uint32_t a;
			uint32_t b;
			uint32_t c;
			uint32_t d;
			__get_cpuid( func, &a, &b, &c, &d );
			p_data[0] = int32_t( a );
			p_data[1] = int32_t( b );
			p_data[2] = int32_t( c );
			p_data[3] = int32_t( d );
		}

#endif

		uint32_t getCoreCount()
		{
			SYSTEM_INFO sysinfo = { 0 };
			::GetSystemInfo( &sysinfo );
			return uint32_t( sysinfo.dwNumberOfProcessors );
		}
	}
}

#endif
