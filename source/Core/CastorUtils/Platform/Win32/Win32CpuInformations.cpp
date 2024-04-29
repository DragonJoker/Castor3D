#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformWindows )

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/CpuInformations.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <Windows.h>

namespace castor::platform
{
	uint32_t getCoreCount()
	{
		SYSTEM_INFO sysinfo = { 0 };
		::GetSystemInfo( &sysinfo );
		return uint32_t( sysinfo.dwNumberOfProcessors );
	}
}

#if defined( CU_ArchX86_64 ) || defined( CU_ArchX86_32 )
#	include <intrin.h>

namespace castor::platform
{
	namespace win32_x86
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

#	if defined( CU_CompilerMSVC )

		static void callCpuid( uint32_t func, Array< int32_t, 4 > & data )
		{
			__cpuid( data.data(), int( func ) );
		}

#	else

		static void callCpuid( uint32_t func, Array< int32_t, 4 > & data )
		{
			uint32_t a;
			uint32_t b;
			uint32_t c;
			uint32_t d;
			__get_cpuid( func, &a, &b, &c, &d );
			data[0] = int32_t( a );
			data[1] = int32_t( b );
			data[2] = int32_t( c );
			data[3] = int32_t( d );
		}

#	endif
	}

	String getCPUModel()
	{
		Array< Array< int32_t, 4 >, 3 > brand{};
		win32_x86::callCpuid( 0x80000002, brand[0] );
		win32_x86::callCpuid( 0x80000003, brand[1] );
		win32_x86::callCpuid( 0x80000004, brand[2] );
		auto result = win32_x86::makeString( brand[0][0] );
		result += win32_x86::makeString( brand[0][1] );
		result += win32_x86::makeString( brand[0][2] );
		result += win32_x86::makeString( brand[0][3] );
		result += win32_x86::makeString( brand[1][0] );
		result += win32_x86::makeString( brand[1][1] );
		result += win32_x86::makeString( brand[1][2] );
		result += win32_x86::makeString( brand[1][3] );
		result += win32_x86::makeString( brand[2][0] );
		result += win32_x86::makeString( brand[2][1] );
		result += win32_x86::makeString( brand[2][2] );
		result += win32_x86::makeString( brand[2][3] );
		return string::trim( result );
	}

	String getCPUVendor()
	{
		Vector< Array< int32_t, 4 > > datas{};
		Array< int32_t, 4 > data;
		win32_x86::callCpuid( 0u, data );
		auto ids = data[0];

		for ( int32_t i = 0; i < ids; ++i )
		{
			win32_x86::callCpuid( uint32_t( i ), data );
			datas.push_back( data );
		}

		String result;

		if ( !datas.empty() )
		{
			result = win32_x86::makeString( datas[0][1] );
			result += win32_x86::makeString( datas[0][3] );
			result += win32_x86::makeString( datas[0][2] );
		}

		return string::trim( result );
	}
}

#else

namespace castor::platform
{
	String getCPUModel()
	{
		return String{};
	}

	String getCPUVendor()
	{
		return String{};
	}
}

#endif

#endif
