#include "Config/PlatformConfig.hpp"

#if defined( CASTOR_PLATFORM_LINUX )

#include "Miscellaneous/CpuInformations.hpp"

#include "Exception/Assertion.hpp"

#include <cpuid.h>

namespace Castor
{
	namespace Platform
	{
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

		uint32_t get_core_count()
		{
			char res[128];
			FILE * fp = popen( "/bin/cat /proc/cpuinfo | grep -c '^processor'", "r" );
			auto read = fread( res, 1, sizeof( res ) - 1, fp ) < sizeof( res );
			ENSURE( read && read < sizeof( res ) );
			pclose( fp );
			return uint32_t( res[0] );
		}
	}
}

#endif
