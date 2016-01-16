#include "CpuInformations.hpp"

#include "Assertion.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <X11/Xlib.h>
#endif

namespace Castor
{
#if defined( __GNUG__ )

#	define call_cpuid( func, ax, bx, cx, dx )\
	__asm__ __volatile__ ( "cpuid": "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));

#elif defined( _MSC_VER )

	void call_cpuid( uint32_t func, int32_t & p_a, int32_t & p_b, int32_t & p_c, int32_t & p_d )
	{
		int32_t l_a = 0;
		int32_t l_b = 0;
		int32_t l_c = 0;
		int32_t l_d = 0;

		__asm
		{
			mov	eax, func
			cpuid
			mov	l_a, eax
			mov	l_b, ebx
			mov	l_c, ecx
			mov	l_d, edx
		}

		p_a = l_a;
		p_b = l_b;
		p_c = l_c;
		p_d = l_d;
	}

#endif

	CpuInformations::CpuInformations()
		: m_mmx( false )
		, m_sse( false )
		, m_sse2( false )
		, m_sse3( false )
	{
#define MAKE_STRING( v ) std::string( { char( ( v >> 0 ) & 0xFF ), char( ( v >> 8 ) & 0xFF ), char( ( v >> 16 ) & 0xFF ), char( ( v >> 24 ) & 0xFF ) } )
#define GET_BIT( v, i ) ( ( v >> i ) & 0x01 )

		int32_t a = 0;
		int32_t b = 0;
		int32_t c = 0;
		int32_t d = 0;
		call_cpuid( 0x00000000, a, b, c, d );
		m_name = MAKE_STRING( b );
		m_name += MAKE_STRING( d );
		m_name += MAKE_STRING( c );

		call_cpuid( 0x00000001, a, b, c, d );
		m_mmx = GET_BIT( d, 23 ) != 0;
		m_sse = GET_BIT( d, 0x19 ) != 0;
		m_sse2 = GET_BIT( d, 0x1A ) != 0;
		m_sse3 = GET_BIT( c, 0x00 ) != 0;

#undef GET_BIT
#undef MAKE_STRING
	}

	CpuInformations::~CpuInformations()
	{
	}

#if defined( _WIN32 )

	uint8_t CpuInformations::GetCoreCount()
	{
		SYSTEM_INFO sysinfo = { 0 };
		::GetSystemInfo( &sysinfo );
		return uint8_t( sysinfo.dwNumberOfProcessors );
	}

#elif defined( __linux__ )

	uint8_t CpuInformations::GetCoreCount()
	{
		char res[128];
		FILE * fp = popen( "/bin/cat /proc/cpuinfo |grep -c '^processor'", "r" );
		ENSURE( fread( res, 1, sizeof( res ) - 1, fp ) < sizeof( res ) );
		pclose( fp );
		return res[0];
	}

#else
#	error "Yet unsupported OS"
#endif
}
