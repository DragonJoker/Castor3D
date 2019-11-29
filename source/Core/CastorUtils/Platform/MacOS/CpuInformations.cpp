#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformApple )

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/CpuInformations.hpp"

#include <cpuid.h>
#include <sys/sysctl.h>

namespace castor
{
	namespace Platform
	{
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

		uint32_t getCoreCount()
		{
			int mib[4];
			int numCPU;
			size_t len = sizeof( numCPU ); 

			/* set the mib for hw.ncpu */
			mib[0] = CTL_HW;
			mib[1] = HW_AVAILCPU;

			/* get the number of CPUs from the system */
			sysctl( mib, 2, &numCPU, &len, NULL, 0 );

			if ( numCPU < 1 )
			{
				mib[1] = HW_NCPU;
				sysctl( mib, 2, &numCPU, &len, NULL, 0 );

				if ( numCPU < 1 )
				{
					numCPU = 1;
				}
			}

			return uint32_t( numCPU );
		}
	}
}

#endif
