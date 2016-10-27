#include "CastorUtilsStringTest.hpp"

using namespace Castor;

namespace Testing
{
	//*********************************************************************************************

	namespace
	{
		template< typename InChar, typename OutChar >
		inline void convert( std::basic_string< InChar > const & p_strIn, std::basic_string< OutChar > & p_strOut, std::locale const & p_locale = std::locale() )
		{
			if ( !p_strIn.empty() )
			{
				typedef typename std::codecvt< OutChar, InChar, std::mbstate_t > facet_type;
				typedef typename facet_type::result result_type;
				std::mbstate_t l_state = std::mbstate_t();
				result_type l_result;
				std::vector< OutChar > l_buffer( p_strIn.size() );
				InChar const * l_pEndIn = NULL;
				OutChar * l_pEndOut = NULL;
				l_result = std::use_facet< facet_type >( p_locale ).in( l_state,
						   p_strIn.data(),		p_strIn.data() + p_strIn.length(),		l_pEndIn,
						   &l_buffer.front(),	&l_buffer.front() + l_buffer.size(),	l_pEndOut
																	  );
				p_strOut = std::basic_string< OutChar >( &l_buffer.front(), l_pEndOut );
			}
			else
			{
				p_strOut.clear();
			}
		}

		inline void narrow( std::wstring const & p_wstrIn, std::string & p_strOut )
		{
			p_strOut.resize( p_wstrIn.size() + 1, '\0' );
			std::ostringstream l_stream;
			std::ctype< wchar_t > const & l_ctfacet = std::use_facet< std::ctype< wchar_t > >( l_stream.getloc() );

			if ( p_wstrIn.size() > 0 )
			{
				l_ctfacet.narrow( p_wstrIn.data(), p_wstrIn.data() + p_wstrIn.size(), '?', &p_strOut[0] );
			}
		}

		inline void widen( std::string const & p_strIn, std::wstring & p_wstrOut )
		{
			p_wstrOut.resize( p_strIn.size() + 1, L'\0' );
			std::wostringstream l_wstream;
			std::ctype< wchar_t > const & l_ctfacet = std::use_facet< std::ctype< wchar_t > >( l_wstream.getloc() );

			if ( p_strIn.size() > 0 )
			{
				l_ctfacet.widen( p_strIn.data(), p_strIn.data() + p_strIn.size(), &p_wstrOut[0] );
			}
		}
	}

	//*********************************************************************************************

	CastorUtilsStringTest::CastorUtilsStringTest()
		:	TestCase( "CastorUtilsStringTest" )
	{
	}

	CastorUtilsStringTest::~CastorUtilsStringTest()
	{
	}

	void CastorUtilsStringTest::DoRegisterTests()
	{
		DoRegisterTest( "StringConversions", std::bind( &CastorUtilsStringTest::StringConversions, this ) );
	}

	void CastorUtilsStringTest::StringConversions()
	{
		String l_tstrOut;
		String l_tstrIn( cuT( "STR : Bonjoir éêèàÉÊÈÀ" ) );
		std::string l_strIn = "STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring l_wstrIn = L"STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring l_wstrOut;
		std::string l_strOut;
		l_tstrOut = string::string_cast< xchar >( l_strIn );
		std::cout << "	Conversion from std::string to String" << std::endl;
		std::cout << "		Entry  : " << l_strIn << std::endl;
		std::cout << "		Result : " << l_tstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( l_tstrOut, l_tstrIn );
		l_tstrOut = string::string_cast< xchar >( l_wstrIn );
		std::cout << "	Conversion from std::wstring to String" << std::endl;
		std::wcout << L"		Entry  : " << l_wstrIn << std::endl;
		std::cout << "		Result : " << l_tstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( l_tstrOut, l_tstrIn );
		l_strOut = string::string_cast< char >( l_tstrIn );
		std::cout << "	Conversion from String to std::string" << std::endl;
		std::cout << "		Entry  : " << l_tstrIn << std::endl;
		std::cout << "		Result : " << l_strOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( l_strOut, l_strIn );
		l_wstrOut = string::string_cast< wchar_t >( l_tstrIn );
		std::cout << "	Conversion from String to std::wstring" << std::endl;
		std::cout << "		Entry  : " << l_tstrIn << std::endl;
		std::wcout << L"		Result : " << l_wstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( l_wstrOut, l_wstrIn );
		convert( l_strIn, l_wstrOut );
		std::cout << "	Conversion from std::string to std::wstring" << std::endl;
		std::cout << "		Entry  : " << l_strIn << std::endl;
		std::wcout << L"		Result : " << l_wstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( l_wstrOut, l_wstrIn );
		convert( l_wstrIn, l_strOut );
		std::cout << "	Conversion from std::wstring to std::string" << std::endl;
		std::wcout << L"		Entry  : " << l_wstrIn << std::endl;
		std::cout << "		Result : " << l_strOut << std::endl;
		CT_EQUAL( l_strOut, l_strIn );
	}

	//*********************************************************************************************

	CastorUtilsStringBench::CastorUtilsStringBench()
		: BenchCase( "CastorUtilsStringBench" )
		, m_strIn( "STR : Bonjoir éêèàÉÊÈÀ" )
		, m_wstrIn( L"STR : Bonjoir éêèàÉÊÈÀ" )
	{
	}

	CastorUtilsStringBench::~CastorUtilsStringBench()
	{
	}

	void CastorUtilsStringBench::Execute()
	{
		BENCHMARK( StrToWStrUsingConvert, NB_TESTS );
		BENCHMARK( StrToWStrUsingWiden, NB_TESTS );
		BENCHMARK( WStrToStrUsingConvert, NB_TESTS );
		BENCHMARK( WStrToStrUsingNarrow, NB_TESTS );
	}

	void CastorUtilsStringBench::StrToWStrUsingConvert()
	{
		convert( m_strIn, m_wstrOut );
	}

	void CastorUtilsStringBench::StrToWStrUsingWiden()
	{
		widen( m_strIn, m_wstrOut );
	}

	void CastorUtilsStringBench::WStrToStrUsingConvert()
	{
		convert( m_wstrIn, m_strOut );
	}

	void CastorUtilsStringBench::WStrToStrUsingNarrow()
	{
		narrow( m_wstrIn, m_strOut );
	}

	//*********************************************************************************************
}
