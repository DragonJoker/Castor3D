#include "CpuInformations.hpp"

#include "Exception/Assertion.hpp"

namespace Castor
{
	namespace Platform
	{
		void call_cpuid( uint32_t func, std::array< int32_t, 4 > & p_data );
		uint32_t get_core_count();
	}

	CpuInformations::CpuInformationsInternal::CpuInformationsInternal()
	{
		auto makeString = []( int32_t v )
		{
			return std::string( {
				char( ( v >> 0 ) & 0xFF ),
				char( ( v >> 8 ) & 0xFF ),
				char( ( v >> 16 ) & 0xFF ),
				char( ( v >> 24 ) & 0xFF )
			} );
		};

		std::vector< std::array< int, 4 > > datas{};
		std::array< int32_t, 4 > data;
		Platform::call_cpuid( 0u, data );
		auto ids = data[0];

		for ( int32_t i = 0; i < ids; ++i )
		{
			Platform::call_cpuid( i, data );
			datas.push_back( data );
		}

		m_vendor = makeString( datas[0][1] );
		m_vendor += makeString( datas[0][3] );
		m_vendor += makeString( datas[0][2] );

		if ( m_vendor == "GenuineIntel" )
		{
			m_isIntel = true;
		}
		else if ( m_vendor == "AuthenticAMD" )
		{
			m_isAMD = true;
		}

		// load bitset with flags for function 0x00000001
		if ( ids >= 1 )
		{
			m_f_1_ECX = datas[1][2];
			m_f_1_EDX = datas[1][3];
		}

		// load bitset with flags for function 0x00000007
		if ( ids >= 7 )
		{
			m_f_7_EBX = datas[7][1];
			m_f_7_ECX = datas[7][2];
		}

		m_coreCount = Platform::get_core_count();
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
		auto support = []( bool p_supported )
		{
			return ( p_supported ? "supported" : "not supported" );
		};

		p_stream << "CPU informations:" << std::endl;
		p_stream << "    Vendor: " << p_object.Vendor() << std::endl;
		p_stream << "    Core count: " << p_object.CoreCount() << std::endl;
		p_stream << "    ADX: " << support( p_object.ADX() ) << std::endl;
		p_stream << "    AES: " << support( p_object.AES() ) << std::endl;
		p_stream << "    AVX: " << support( p_object.AVX() ) << std::endl;
		p_stream << "    AVX2: " << support( p_object.AVX2() ) << std::endl;
		p_stream << "    AVX512CD: " << support( p_object.AVX512CD() ) << std::endl;
		p_stream << "    AVX512ER: " << support( p_object.AVX512ER() ) << std::endl;
		p_stream << "    AVX512F: " << support( p_object.AVX512F() ) << std::endl;
		p_stream << "    AVX512PF: " << support( p_object.AVX512PF() ) << std::endl;
		p_stream << "    BMI1: " << support( p_object.BMI1() ) << std::endl;
		p_stream << "    BMI2: " << support( p_object.BMI2() ) << std::endl;
		p_stream << "    CLFSH: " << support( p_object.CLFSH() ) << std::endl;
		p_stream << "    CMPXCHG16B: " << support( p_object.CMPXCHG16B() ) << std::endl;
		p_stream << "    CX8: " << support( p_object.CX8() ) << std::endl;
		p_stream << "    ERMS: " << support( p_object.ERMS() ) << std::endl;
		p_stream << "    F16C: " << support( p_object.F16C() ) << std::endl;
		p_stream << "    FMA: " << support( p_object.FMA() ) << std::endl;
		p_stream << "    FSGSBASE: " << support( p_object.FSGSBASE() ) << std::endl;
		p_stream << "    FXSR: " << support( p_object.FXSR() ) << std::endl;
		p_stream << "    HLE: " << support( p_object.HLE() ) << std::endl;
		p_stream << "    INVPCID: " << support( p_object.INVPCID() ) << std::endl;
		p_stream << "    MMX: " << support( p_object.MMX() ) << std::endl;
		p_stream << "    MONITOR: " << support( p_object.MONITOR() ) << std::endl;
		p_stream << "    MOVBE: " << support( p_object.MOVBE() ) << std::endl;
		p_stream << "    MSR: " << support( p_object.MSR() ) << std::endl;
		p_stream << "    OSXSAVE: " << support( p_object.OSXSAVE() ) << std::endl;
		p_stream << "    PCLMULQDQ: " << support( p_object.PCLMULQDQ() ) << std::endl;
		p_stream << "    POPCNT: " << support( p_object.POPCNT() ) << std::endl;
		p_stream << "    PREFETCHWT1: " << support( p_object.PREFETCHWT1() ) << std::endl;
		p_stream << "    RDRAND: " << support( p_object.RDRAND() ) << std::endl;
		p_stream << "    RDSEED: " << support( p_object.RDSEED() ) << std::endl;
		p_stream << "    RTM: " << support( p_object.RTM() ) << std::endl;
		p_stream << "    SEP: " << support( p_object.SEP() ) << std::endl;
		p_stream << "    SHA: " << support( p_object.SHA() ) << std::endl;
		p_stream << "    SSE: " << support( p_object.SSE() ) << std::endl;
		p_stream << "    SSE2: " << support( p_object.SSE2() ) << std::endl;
		p_stream << "    SSE3: " << support( p_object.SSE3() ) << std::endl;
		p_stream << "    SSE4.1: " << support( p_object.SSE41() ) << std::endl;
		p_stream << "    SSE4.2: " << support( p_object.SSE42() ) << std::endl;
		p_stream << "    SSSE3: " << support( p_object.SSSE3() ) << std::endl;
		p_stream << "    XSAVE: " << support( p_object.XSAVE() );
		return p_stream;
	}
}
