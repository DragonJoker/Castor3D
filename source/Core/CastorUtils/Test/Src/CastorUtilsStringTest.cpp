#include "CastorUtilsStringTest.hpp"

using namespace castor;

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
				std::mbstate_t state = std::mbstate_t();
				result_type result;
				std::vector< OutChar > buffer( p_strIn.size() );
				InChar const * pEndIn = NULL;
				OutChar * pEndOut = NULL;
				result = std::use_facet< facet_type >( p_locale ).in( state,
						   p_strIn.data(),		p_strIn.data() + p_strIn.length(),		pEndIn,
						   &buffer.front(),	&buffer.front() + buffer.size(),	pEndOut
																	  );
				p_strOut = std::basic_string< OutChar >( &buffer.front(), pEndOut );
			}
			else
			{
				p_strOut.clear();
			}
		}

		inline void narrow( std::wstring const & p_wstrIn, std::string & p_strOut )
		{
			p_strOut.resize( p_wstrIn.size() + 1, '\0' );
			std::ostringstream stream;
			std::ctype< wchar_t > const & ctfacet = std::use_facet< std::ctype< wchar_t > >( stream.getloc() );

			if ( p_wstrIn.size() > 0 )
			{
				ctfacet.narrow( p_wstrIn.data(), p_wstrIn.data() + p_wstrIn.size(), '?', &p_strOut[0] );
			}
		}

		inline void widen( std::string const & p_strIn, std::wstring & p_wstrOut )
		{
			p_wstrOut.resize( p_strIn.size() + 1, L'\0' );
			std::wostringstream wstream;
			std::ctype< wchar_t > const & ctfacet = std::use_facet< std::ctype< wchar_t > >( wstream.getloc() );

			if ( p_strIn.size() > 0 )
			{
				ctfacet.widen( p_strIn.data(), p_strIn.data() + p_strIn.size(), &p_wstrOut[0] );
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

	void CastorUtilsStringTest::doRegisterTests()
	{
		doRegisterTest( "StringConversions", std::bind( &CastorUtilsStringTest::StringConversions, this ) );
	}

	void CastorUtilsStringTest::StringConversions()
	{
		String tstrOut;
		String tstrIn( cuT( "STR : Bonjoir éêèàÉÊÈÀ" ) );
		std::string strIn = "STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring wstrIn = L"STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring wstrOut;
		std::string strOut;
		tstrOut = string::stringCast< xchar >( strIn );
		std::cout << "	Conversion from std::string to String" << std::endl;
		std::cout << "		Entry  : " << strIn << std::endl;
		std::cout << "		Result : " << tstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( tstrOut, tstrIn );
		tstrOut = string::stringCast< xchar >( wstrIn );
		std::cout << "	Conversion from std::wstring to String" << std::endl;
		std::wcout << L"		Entry  : " << wstrIn << std::endl;
		std::cout << "		Result : " << tstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( tstrOut, tstrIn );
		strOut = string::stringCast< char >( tstrIn );
		std::cout << "	Conversion from String to std::string" << std::endl;
		std::cout << "		Entry  : " << tstrIn << std::endl;
		std::cout << "		Result : " << strOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( strOut, strIn );
		wstrOut = string::stringCast< wchar_t >( tstrIn );
		std::cout << "	Conversion from String to std::wstring" << std::endl;
		std::cout << "		Entry  : " << tstrIn << std::endl;
		std::wcout << L"		Result : " << wstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( wstrOut, wstrIn );
		convert( strIn, wstrOut );
		std::cout << "	Conversion from std::string to std::wstring" << std::endl;
		std::cout << "		Entry  : " << strIn << std::endl;
		std::wcout << L"		Result : " << wstrOut << std::endl;
		std::cout << std::endl;
		CT_EQUAL( wstrOut, wstrIn );
		convert( wstrIn, strOut );
		std::cout << "	Conversion from std::wstring to std::string" << std::endl;
		std::wcout << L"		Entry  : " << wstrIn << std::endl;
		std::cout << "		Result : " << strOut << std::endl;
		CT_EQUAL( strOut, strIn );
	}

	//*********************************************************************************************
}
