#include "CastorUtils/Config/PlatformConfig.hpp"

#if defined( CU_PlatformLinux )

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/CpuInformations.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <cpuid.h>
#include <sstream>
#include <fstream>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"

namespace castor::platform
{
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

	String getCPUModel()
	{
		MbString line;
		std::ifstream finfo( "/proc/cpuinfo" );
		MbString result;

		while ( std::getline( finfo, line ) )
		{
			MbStringStream str( line );
			MbString itype;
			MbString info;

			if ( std::getline( str, itype, ':' )
				&& std::getline( str, info )
				&& itype.substr( 0, 10 ) == "model name" )
			{
				result = info.substr( 1 );
				break;
			}
		}

		return makeString( result );
	}
}

#if defined( CU_ArchX86_64 ) || defined( CU_ArchX86_32 )
#	include <cpuid.h>

namespace castor::platform
{
	namespace linux_x86
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
		linux_x86::callCpuid( 0u, data );
		auto ids = data[0];

		for ( int32_t i = 0; i < ids; ++i )
		{
			linux_x86::callCpuid( uint32_t( i ), data );
			datas.push_back( data );
		}

		String result;

		if ( !datas.empty() )
		{
			result = linux_x86::makeString( datas[0][1] );
			result += linux_x86::makeString( datas[0][3] );
			result += linux_x86::makeString( datas[0][2] );
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
