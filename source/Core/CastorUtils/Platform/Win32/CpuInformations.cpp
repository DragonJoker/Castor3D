#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/CpuInformations.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <Windows.h>
#include <intrin.h>

namespace castor
{
	namespace Platform
	{
#if defined( CU_CompilerMSVC )

		void callCpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
		{
			__cpuid( p_data.data(), int( func ) );
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

		std::string getCPUModel()
		{
			auto makeString = []( int32_t v )
			{
				std::string result;
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

				return result;
			};

			std::array< std::array< int32_t, 4 >, 3 > brand;
			Platform::callCpuid( 0x80000002, brand[0] );
			Platform::callCpuid( 0x80000003, brand[1] );
			Platform::callCpuid( 0x80000004, brand[2] );
			auto result = makeString( brand[0][0] );
			result += makeString( brand[0][1] );
			result += makeString( brand[0][2] );
			result += makeString( brand[0][3] );
			result += makeString( brand[1][0] );
			result += makeString( brand[1][1] );
			result += makeString( brand[1][2] );
			result += makeString( brand[1][3] );
			result += makeString( brand[2][0] );
			result += makeString( brand[2][1] );
			result += makeString( brand[2][2] );
			result += makeString( brand[2][3] );
			return string::trim( result );
		}
	}
}

#endif
