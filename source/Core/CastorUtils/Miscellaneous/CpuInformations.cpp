#include "CastorUtils/Miscellaneous/CpuInformations.hpp"

#include "CastorUtils/Exception/Assertion.hpp"

namespace castor
{
	namespace Platform
	{
		void callCpuid( uint32_t func, std::array< int32_t, 4 > & p_data );
		uint32_t getCoreCount();
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
		Platform::callCpuid( 0u, data );
		auto ids = data[0];

		for ( int32_t i = 0; i < ids; ++i )
		{
			Platform::callCpuid( i, data );
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

		m_coreCount = Platform::getCoreCount();
	}

	CpuInformations::CpuInformationsInternal const CpuInformations::m_internal;

	CpuInformations::CpuInformations()
	{
	}

	CpuInformations::~CpuInformations()
	{
	}

	std::ostream & operator<<( std::ostream & stream, CpuInformations const & object )
	{
		auto support = []( bool supported )
		{
			return ( supported ? "supported" : "not supported" );
		};

		stream << "CPU informations:" << std::endl;
		stream << "    Vendor: " << object.getVendor() << std::endl;
		stream << "    Core count: " << object.getCoreCount() << std::endl;
		stream << "    ADX: " << support( object.ADX() ) << std::endl;
		stream << "    AES: " << support( object.AES() ) << std::endl;
		stream << "    AVX: " << support( object.AVX() ) << std::endl;
		stream << "    AVX2: " << support( object.AVX2() ) << std::endl;
		stream << "    AVX512CD: " << support( object.AVX512CD() ) << std::endl;
		stream << "    AVX512ER: " << support( object.AVX512ER() ) << std::endl;
		stream << "    AVX512F: " << support( object.AVX512F() ) << std::endl;
		stream << "    AVX512PF: " << support( object.AVX512PF() ) << std::endl;
		stream << "    BMI1: " << support( object.BMI1() ) << std::endl;
		stream << "    BMI2: " << support( object.BMI2() ) << std::endl;
		stream << "    CLFSH: " << support( object.CLFSH() ) << std::endl;
		stream << "    CMPXCHG16B: " << support( object.CMPXCHG16B() ) << std::endl;
		stream << "    CX8: " << support( object.CX8() ) << std::endl;
		stream << "    ERMS: " << support( object.ERMS() ) << std::endl;
		stream << "    F16C: " << support( object.F16C() ) << std::endl;
		stream << "    FMA: " << support( object.FMA() ) << std::endl;
		stream << "    FSGSBASE: " << support( object.FSGSBASE() ) << std::endl;
		stream << "    FXSR: " << support( object.FXSR() ) << std::endl;
		stream << "    HLE: " << support( object.HLE() ) << std::endl;
		stream << "    INVPCID: " << support( object.INVPCID() ) << std::endl;
		stream << "    MMX: " << support( object.MMX() ) << std::endl;
		stream << "    MONITOR: " << support( object.MONITOR() ) << std::endl;
		stream << "    MOVBE: " << support( object.MOVBE() ) << std::endl;
		stream << "    MSR: " << support( object.MSR() ) << std::endl;
		stream << "    OSXSAVE: " << support( object.OSXSAVE() ) << std::endl;
		stream << "    PCLMULQDQ: " << support( object.PCLMULQDQ() ) << std::endl;
		stream << "    POPCNT: " << support( object.POPCNT() ) << std::endl;
		stream << "    PREFETCHWT1: " << support( object.PREFETCHWT1() ) << std::endl;
		stream << "    RDRAND: " << support( object.RDRAND() ) << std::endl;
		stream << "    RDSEED: " << support( object.RDSEED() ) << std::endl;
		stream << "    RTM: " << support( object.RTM() ) << std::endl;
		stream << "    SEP: " << support( object.SEP() ) << std::endl;
		stream << "    SHA: " << support( object.SHA() ) << std::endl;
		stream << "    SSE: " << support( object.SSE() ) << std::endl;
		stream << "    SSE2: " << support( object.SSE2() ) << std::endl;
		stream << "    SSE3: " << support( object.SSE3() ) << std::endl;
		stream << "    SSE4.1: " << support( object.SSE41() ) << std::endl;
		stream << "    SSE4.2: " << support( object.SSE42() ) << std::endl;
		stream << "    SSSE3: " << support( object.SSSE3() ) << std::endl;
		stream << "    XSAVE: " << support( object.XSAVE() );
		return stream;
	}
}
