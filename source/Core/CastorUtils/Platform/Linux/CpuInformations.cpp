#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformLinux )

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/CpuInformations.hpp"

#include <cpuid.h>

namespace castor
{
	namespace Platform
	{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"
		void callCpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
		{
			uint32_t a{};
			uint32_t b{};
			uint32_t c{};
			uint32_t d{};
			__get_cpuid( func, &a, &b, &c, &d );
			p_data[0] = int32_t( a );
			p_data[1] = int32_t( b );
			p_data[2] = int32_t( c );
			p_data[3] = int32_t( d );
		}

		uint32_t getCoreCount()
		{
			char res[128];
			FILE * fp = popen( "/bin/cat /proc/cpuinfo | grep -c '^processor'", "r" );
			auto read = fread( res, 1, sizeof( res ) - 1, fp );

			if ( !read || read >= sizeof( res ) )
			{
				CU_Failure( "Couldn't retrieve CPU cores count" );
			}

			pclose( fp );
			return uint32_t( res[0] );
		}
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
	}
}

#endif
