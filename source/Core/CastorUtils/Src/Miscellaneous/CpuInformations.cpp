#include "CpuInformations.hpp"

#include "Exception/Assertion.hpp"

#if defined( _WIN32 )
#	include <intrin.h>
#else
#	include <X11/Xlib.h>
#	include <cpuid.h>
#endif

namespace Castor
{
#if defined( __GNUG__ )

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

#elif defined( _MSC_VER )

	void call_cpuid( uint32_t func, std::array< int32_t, 4 > & p_data )
	{
		__cpuid( p_data.data(), func );
	}

#endif
#if defined( _WIN32 )

	inline uint32_t get_core_count()
	{
		SYSTEM_INFO sysinfo = { 0 };
		::GetSystemInfo( &sysinfo );
		return uint32_t( sysinfo.dwNumberOfProcessors );
	}

#elif defined( __linux__ )

	inline uint32_t get_core_count()
	{
		char res[128];
		FILE * fp = popen( "/bin/cat /proc/cpuinfo | grep -c '^processor'", "r" );
		ENSURE( fread( res, 1, sizeof( res ) - 1, fp ) < sizeof( res ) );
		pclose( fp );
		return uint32_t( res[0] );
	}

#else
#	error "Yet unsupported OS"
#endif

	CpuInformations::CpuInformationsInternal::CpuInformationsInternal()
	{
		auto l_makeString = []( int32_t v )
		{
			return std::string( {
				char( ( v >> 0 ) & 0xFF ),
				char( ( v >> 8 ) & 0xFF ),
				char( ( v >> 16 ) & 0xFF ),
				char( ( v >> 24 ) & 0xFF )
			} );
		};

		std::vector< std::array< int, 4 > > l_datas{};
		std::array< int32_t, 4 > l_data;
		call_cpuid( 0u, l_data );
		auto l_ids = l_data[0];

		for ( int32_t i = 0; i < l_ids; ++i )
		{
			call_cpuid( i, l_data );
			l_datas.push_back( l_data );
		}

		m_vendor = l_makeString( l_datas[0][1] );
		m_vendor += l_makeString( l_datas[0][3] );
		m_vendor += l_makeString( l_datas[0][2] );

		if ( m_vendor == "GenuineIntel" )
		{
			m_isIntel = true;
		}
		else if ( m_vendor == "AuthenticAMD" )
		{
			m_isAMD = true;
		}

		// load bitset with flags for function 0x00000001
		if ( l_ids >= 1 )
		{
			m_f_1_ECX = l_datas[1][2];
			m_f_1_EDX = l_datas[1][3];
		}

		// load bitset with flags for function 0x00000007
		if ( l_ids >= 7 )
		{
			m_f_7_EBX = l_datas[7][1];
			m_f_7_ECX = l_datas[7][2];
		}

		m_coreCount = get_core_count();
	}

	CpuInformations::CpuInformationsInternal const CpuInformations::m_internal;

	CpuInformations::CpuInformations()
	{
	}

	CpuInformations::~CpuInformations()
	{
	}

	std::ostream & operator<<( std::ostream & p_stream, CpuInformations const & p_object )
	{
		auto l_support = []( bool p_supported )
		{
			return ( p_supported ? "supported" : "not supported" );
		};

		p_stream << "CPU informations:" << std::endl;
		p_stream << "    Vendor: " << p_object.Vendor() << std::endl;
		p_stream << "    Core count: " << p_object.CoreCount() << std::endl;
		p_stream << "    ADX: " << l_support( p_object.ADX() ) << std::endl;
		p_stream << "    AES: " << l_support( p_object.AES() ) << std::endl;
		p_stream << "    AVX: " << l_support( p_object.AVX() ) << std::endl;
		p_stream << "    AVX2: " << l_support( p_object.AVX2() ) << std::endl;
		p_stream << "    AVX512CD: " << l_support( p_object.AVX512CD() ) << std::endl;
		p_stream << "    AVX512ER: " << l_support( p_object.AVX512ER() ) << std::endl;
		p_stream << "    AVX512F: " << l_support( p_object.AVX512F() ) << std::endl;
		p_stream << "    AVX512PF: " << l_support( p_object.AVX512PF() ) << std::endl;
		p_stream << "    BMI1: " << l_support( p_object.BMI1() ) << std::endl;
		p_stream << "    BMI2: " << l_support( p_object.BMI2() ) << std::endl;
		p_stream << "    CLFSH: " << l_support( p_object.CLFSH() ) << std::endl;
		p_stream << "    CMPXCHG16B: " << l_support( p_object.CMPXCHG16B() ) << std::endl;
		p_stream << "    CX8: " << l_support( p_object.CX8() ) << std::endl;
		p_stream << "    ERMS: " << l_support( p_object.ERMS() ) << std::endl;
		p_stream << "    F16C: " << l_support( p_object.F16C() ) << std::endl;
		p_stream << "    FMA: " << l_support( p_object.FMA() ) << std::endl;
		p_stream << "    FSGSBASE: " << l_support( p_object.FSGSBASE() ) << std::endl;
		p_stream << "    FXSR: " << l_support( p_object.FXSR() ) << std::endl;
		p_stream << "    HLE: " << l_support( p_object.HLE() ) << std::endl;
		p_stream << "    INVPCID: " << l_support( p_object.INVPCID() ) << std::endl;
		p_stream << "    MMX: " << l_support( p_object.MMX() ) << std::endl;
		p_stream << "    MONITOR: " << l_support( p_object.MONITOR() ) << std::endl;
		p_stream << "    MOVBE: " << l_support( p_object.MOVBE() ) << std::endl;
		p_stream << "    MSR: " << l_support( p_object.MSR() ) << std::endl;
		p_stream << "    OSXSAVE: " << l_support( p_object.OSXSAVE() ) << std::endl;
		p_stream << "    PCLMULQDQ: " << l_support( p_object.PCLMULQDQ() ) << std::endl;
		p_stream << "    POPCNT: " << l_support( p_object.POPCNT() ) << std::endl;
		p_stream << "    PREFETCHWT1: " << l_support( p_object.PREFETCHWT1() ) << std::endl;
		p_stream << "    RDRAND: " << l_support( p_object.RDRAND() ) << std::endl;
		p_stream << "    RDSEED: " << l_support( p_object.RDSEED() ) << std::endl;
		p_stream << "    RTM: " << l_support( p_object.RTM() ) << std::endl;
		p_stream << "    SEP: " << l_support( p_object.SEP() ) << std::endl;
		p_stream << "    SHA: " << l_support( p_object.SHA() ) << std::endl;
		p_stream << "    SSE: " << l_support( p_object.SSE() ) << std::endl;
		p_stream << "    SSE2: " << l_support( p_object.SSE2() ) << std::endl;
		p_stream << "    SSE3: " << l_support( p_object.SSE3() ) << std::endl;
		p_stream << "    SSE4.1: " << l_support( p_object.SSE41() ) << std::endl;
		p_stream << "    SSE4.2: " << l_support( p_object.SSE42() ) << std::endl;
		p_stream << "    SSSE3: " << l_support( p_object.SSSE3() ) << std::endl;
		p_stream << "    XSAVE: " << l_support( p_object.XSAVE() );
		return p_stream;
	}
}
