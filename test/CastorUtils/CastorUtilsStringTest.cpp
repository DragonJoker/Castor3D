#include "CastorUtilsStringTest.hpp"

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
				using facet_type = std::codecvt< OutChar, InChar, std::mbstate_t >;
				std::mbstate_t state{};
				castor::Vector< OutChar > buffer( strIn.size() );
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
		doRegisterTest( "StringConversions", [this](){ StringConversions(); } );
	}

	void CastorUtilsStringTest::StringConversions()
	{
		castor::String tstrRef = cuT( "STR : Bonjoir éêèàÉÊÈÀ" );
		std::string strRef = "STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring wstrRef = L"STR : Bonjoir éêèàÉÊÈÀ";
		std::u32string u32strRef = U"STR : Bonjoir éêèàÉÊÈÀ";
		{
			CT_ON( "Conversion from std::string to String" );
			auto out = castor::makeString( strRef );
			CT_WHEN( "Entry  = " + strRef );
			CT_AND( cuT( " Result = " ) + out );
			CT_EQUAL( out, tstrRef );
		}
		{
			CT_ON( "Conversion from std::wstring to String" );
			auto out = castor::makeString( wstrRef );
			CT_WHEN( L"Entry  = " + wstrRef );
			CT_AND( cuT( " Result = " ) + out );
			CT_EQUAL( out, tstrRef );
		}
		{
			CT_ON( "Conversion from std::u32string to String" );
			auto out = castor::makeString( u32strRef );
			CT_WHEN( U"Entry  = " + u32strRef );
			CT_AND( cuT( " Result = " ) + out );
			CT_EQUAL( out, tstrRef );
		}
		{
			CT_ON( "Conversion from String to std::string" );
			auto out = castor::toUtf8( tstrRef );
			CT_WHEN( cuT( "Entry  = " ) + tstrRef );
			CT_AND( " Result = " + out );
			CT_EQUAL( out, strRef );
		}
		{
			CT_ON( "Conversion from String to std::wstring" );
			auto out = castor::toSystemWide( tstrRef );
			CT_WHEN( cuT( "Entry  = " ) + tstrRef );
			CT_AND( L" Result = " + out );
			CT_EQUAL( out, wstrRef );
		}
		{
			CT_ON( "Conversion from String to std::u32string" );
			auto out = castor::toUtf8U32String( tstrRef );
			CT_WHEN( cuT( "Entry  = " ) + tstrRef );
			CT_AND( U" Result = " + out );
			CT_EQUAL( out, u32strRef );
		}
	}

	//*********************************************************************************************
}
