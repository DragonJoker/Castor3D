#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformApple )

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/CpuInformations.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <sys/sysctl.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"

namespace castor::platform
{
	uint32_t getCoreCount()
	{
		Array< int, 4u > mib;
		int numCPU;
		size_t len = sizeof( numCPU );

		/* set the mib for hw.ncpu */
		mib[0] = CTL_HW;
		mib[1] = HW_AVAILCPU;

		/* get the number of CPUs from the system */
		sysctl( mib.data(), 2, &numCPU, &len, nullptr, 0 );

		if ( numCPU < 1 )
		{
			mib[1] = HW_NCPU;
			sysctl( mib.data(), 2, &numCPU, &len, nullptr, 0 );

			if ( numCPU < 1 )
			{
				numCPU = 1;
			}
		}

		return uint32_t( numCPU );
	}

	String getCPUModel()
	{
		Array< char, 1024u > buffer;
		size_t size = sizeof( buffer );
		MbString result;

		if ( sysctlbyname( "machdep.cpu.brand_string", buffer.data(), &size, nullptr, 0 ) >= 0 )
		{
			result = castor::makeString( buffer.data(), size );
		}

		return makeString( result );
	}
}

#if defined( CU_ArchX86_64 ) || defined( CU_ArchX86_32 )
#	include <cpuid.h>

namespace castor::platform
{
	namespace macos_x86
	{
		static String makeString( int32_t v )
		{
			MbString result;
			auto c = char( ( v >> 0 ) & 0xff );

			if ( c )
			{
				result += c;
				c = char( ( v >> 8 ) & 0xff );
			}

			if ( c )
			{
				result += c;
				c = char( ( v >> 16 ) & 0xff );
			}

			if ( c )
			{
				result += c;
				c = char( ( v >> 24 ) & 0xff );
			}

			if ( c )
			{
				result += c;
			}

			return castor::makeString( result );
		};

		static void callCpuid( uint32_t func, Array< int32_t, 4 > & p_data )
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
	}

	String getCPUVendor()
	{
		Vector< Array< int32_t, 4 > > datas{};
		Array< int32_t, 4 > data;
		macos_x86::callCpuid( 0u, data );
		auto ids = data[0];

		for ( int32_t i = 0; i < ids; ++i )
		{
			macos_x86::callCpuid( uint32_t( i ), data );
			datas.push_back( data );
		}

		String result;

		if ( !datas.empty() )
		{
			result = macos_x86::makeString( datas[0][1] );
			result += macos_x86::makeString( datas[0][3] );
			result += macos_x86::makeString( datas[0][2] );
		}

		return string::trim( result );
	}
}

#else

namespace castor::platform
{
	String getCPUVendor()
	{
		return String{};
	}
}

#endif

#pragma GCC diagnostic pop
#pragma clang diagnostic pop

#endif
