#include "CastorUtilsStringTest.hpp"

using namespace castor;

namespace Testing
{
	//*********************************************************************************************

	namespace
	{
		template< typename InChar, typename OutChar >
		inline void convert( std::basic_string< InChar > const & strIn, std::basic_string< OutChar > & strOut, std::locale const & loc = std::locale() )
		{
			if ( !strIn.empty() )
			{
				typedef typename std::codecvt< OutChar, InChar, std::mbstate_t > facet_type;
				std::mbstate_t state = std::mbstate_t();
				std::vector< OutChar > buffer( strIn.size() );
				InChar const * pEndIn = nullptr;
				OutChar * pEndOut = nullptr;
				std::use_facet< facet_type >( loc ).in( state,
					strIn.data(), strIn.data() + strIn.length(), pEndIn,
					 &buffer.front(),	&buffer.front() + buffer.size(), pEndOut );
				strOut = std::basic_string< OutChar >( &buffer.front(), pEndOut );
			}
			else
			{
				strOut.clear();
			}
		}
	}

	//*********************************************************************************************

	CastorUtilsStringTest::CastorUtilsStringTest()
		:	TestCase( "CastorUtilsStringTest" )
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
