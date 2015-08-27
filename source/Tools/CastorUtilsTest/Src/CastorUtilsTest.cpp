#include "CastorUtilsTest.hpp"

#include <Matrix.hpp>
#include <TransformationMatrix.hpp>

#include <glm/glm.hpp>

using namespace Castor;

namespace
{
	inline void randomInit( float * p_pData1, float * p_pData2, int size )
	{
		for ( int i = 0; i < size; ++i )
		{
			p_pData1[i] = rand() / float( RAND_MAX );
			p_pData2[i] = p_pData1[i];
		}
	}

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

	inline bool operator ==( Matrix4x4r const & a, glm::mat4x4 const & b )
	{
		return a[0][0] == b[0][0] && a[1][0] == b[1][0] &&	a[2][0] == b[2][0] &&	a[3][0] == b[3][0]
			   && a[0][1] == b[0][1] && a[1][1] == b[1][1] &&	a[2][1] == b[2][1] &&	a[3][1] == b[3][1]
			   && a[0][2] == b[0][2] && a[1][2] == b[1][2] &&	a[2][2] == b[2][2] &&	a[3][2] == b[3][2]
			   && a[0][3] == b[0][3] && a[1][3] == b[1][3] &&	a[2][3] == b[2][3] &&	a[3][3] == b[3][3];
	}

	inline std::ostream & operator <<( std::ostream & p_stream, glm::mat4 p_mtx )
	{
		for ( uint32_t i = 0; i < 4; i++ )
		{
			for ( uint32_t j = 0; j < 4; j++ )
			{
				p_stream << "\t" << p_mtx[j][i];
			}

			p_stream << std::endl;
		}

		return p_stream;
	}

	inline std::ostream & operator <<( std::ostream & p_stream, Castor::String const & p_str )
	{
		p_stream << Castor::str_utils::to_str( p_str );
		return p_stream;
	}
}

namespace Testing
{
	CastorUtilsTest::CastorUtilsTest()
		:	TestCase( "CastorUtilsTest" )
	{
	}

	CastorUtilsTest::~CastorUtilsTest()
	{
	}

	void CastorUtilsTest::Execute( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		StringConversions( p_errCount, p_testCount );
		MatrixInversion( p_errCount, p_testCount );
		MatrixInversionComparison( p_errCount, p_testCount );
	}

	void CastorUtilsTest::StringConversions( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Logger::LogMessage(	"Test case : StringConversions" );
		String l_tstrOut;
		String l_tstrIn( cuT( "STR : Bonjoir éêèàÉÊÈÀ" ) );
		std::string l_strIn = "STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring l_wstrIn = L"STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring l_wstrOut;
		std::string l_strOut;
		l_tstrOut = str_utils::from_str( l_strIn );
		Logger::LogMessage(	"	Conversion from std::string to String" );
		Logger::LogMessage(	"		Entry  : " + l_strIn );
		Logger::LogMessage( cuT(	"		Result : " ) + l_tstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_tstrOut, l_tstrIn );
		l_tstrOut = str_utils::from_wstr( l_wstrIn );
		Logger::LogMessage(	"	Conversion from std::wstring to String" );
		Logger::LogMessage( L"		Entry  : " + l_wstrIn );
		Logger::LogMessage( cuT(	"		Result : " ) + l_tstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_tstrOut, l_tstrIn );
		l_strOut = str_utils::to_str( l_tstrIn );
		Logger::LogMessage(	"	Conversion from String to std::string" );
		Logger::LogMessage( cuT(	"		Entry  : " ) + l_tstrIn );
		Logger::LogMessage(	"		Result : " + l_strOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_strOut, l_strIn );
		l_wstrOut = str_utils::to_wstr( l_tstrIn );
		Logger::LogMessage(	"	Conversion from String to std::wstring" );
		Logger::LogMessage( cuT(	"		Entry  : " ) + l_tstrIn );
		Logger::LogMessage( L"		Result : " + l_wstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_wstrOut, l_wstrIn );
		convert( l_strIn, l_wstrOut );
		Logger::LogMessage(	"	Conversion from std::string to std::wstring" );
		Logger::LogMessage(	"		Entry  : " + l_strIn );
		Logger::LogMessage( L"		Result : " + l_wstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_wstrOut, l_wstrIn );
		convert( l_wstrIn, l_strOut );
		Logger::LogMessage(	"	Conversion from std::wstring to std::string" );
		Logger::LogMessage( L"		Entry  : " + l_wstrIn );
		Logger::LogMessage(	"		Result : " + l_strOut );
		TEST_EQUAL( l_strOut, l_strIn );
	}

	void CastorUtilsTest::MatrixInversion( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Logger::LogMessage(	"Test case : MatrixInversion" );
		Matrix3x3d l_mtxRGBtoYUV;
		l_mtxRGBtoYUV[0][0] =  0.299;
		l_mtxRGBtoYUV[1][0] =  0.587;
		l_mtxRGBtoYUV[2][0] =  0.114;
		l_mtxRGBtoYUV[0][1] = -0.14713;
		l_mtxRGBtoYUV[1][1] = -0.28886;
		l_mtxRGBtoYUV[2][1] =  0.436;
		l_mtxRGBtoYUV[0][2] =  0.615;
		l_mtxRGBtoYUV[1][2] = -0.51499;
		l_mtxRGBtoYUV[2][2] = -0.10001;
		Logger::LogMessage( cuT( "	RGB to YUV conversion matrix (BT 601) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2] );
		Matrix3x3d l_mtxYUVtoRGB( l_mtxRGBtoYUV.get_inverse() );
		Logger::LogMessage( cuT( "	YUV to RGB conversion matrix (BT 601) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2] );
		TEST_EQUAL( l_mtxRGBtoYUV, l_mtxYUVtoRGB.get_inverse() );
		l_mtxRGBtoYUV[0][0] =  0.2126;
		l_mtxRGBtoYUV[1][0] =  0.7152;
		l_mtxRGBtoYUV[2][0] =  0.0722;
		l_mtxRGBtoYUV[0][1] = -0.09991;
		l_mtxRGBtoYUV[1][1] = -0.33609;
		l_mtxRGBtoYUV[2][1] =  0.436;
		l_mtxRGBtoYUV[0][2] =  0.615;
		l_mtxRGBtoYUV[1][2] = -0.55861;
		l_mtxRGBtoYUV[2][2] = -0.05639;
		Logger::LogMessage( cuT( "	RGB to YUV conversion matrix (BT 709) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2] );
		l_mtxYUVtoRGB = l_mtxRGBtoYUV.get_inverse();
		Logger::LogMessage( cuT( "	YUV to RGB conversion matrix (BT 709) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2] );
		TEST_EQUAL( l_mtxRGBtoYUV, l_mtxYUVtoRGB.get_inverse() );
	}

	void CastorUtilsTest::MatrixInversionComparison( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Logger::LogMessage(	"Test case : MatrixInversionComparison" );
		Matrix4x4r l_mtxRGBtoYUV;
		l_mtxRGBtoYUV[0][0] =  0.299f;
		l_mtxRGBtoYUV[1][0] =  0.587f;
		l_mtxRGBtoYUV[2][0] =  0.114f;
		l_mtxRGBtoYUV[3][0] = 0.0f;
		l_mtxRGBtoYUV[0][1] = -0.14713f;
		l_mtxRGBtoYUV[1][1] = -0.28886f;
		l_mtxRGBtoYUV[2][1] =  0.436f;
		l_mtxRGBtoYUV[3][1] = 0.0f;
		l_mtxRGBtoYUV[0][2] =  0.615f;
		l_mtxRGBtoYUV[1][2] = -0.51499f;
		l_mtxRGBtoYUV[2][2] = -0.10001f;
		l_mtxRGBtoYUV[3][2] = 0.0f;
		l_mtxRGBtoYUV[0][3] =  0.0f;
		l_mtxRGBtoYUV[1][3] = 0.0f;
		l_mtxRGBtoYUV[2][3] = 0.0f;
		l_mtxRGBtoYUV[3][3] = 1.0f;
		glm::mat4x4 l_glmRGBtoYUV;
		l_glmRGBtoYUV[0][0] =  0.299f;
		l_glmRGBtoYUV[1][0] =  0.587f;
		l_glmRGBtoYUV[2][0] =  0.114f;
		l_glmRGBtoYUV[3][0] = 0.0f;
		l_glmRGBtoYUV[0][1] = -0.14713f;
		l_glmRGBtoYUV[1][1] = -0.28886f;
		l_glmRGBtoYUV[2][1] =  0.436f;
		l_glmRGBtoYUV[3][1] = 0.0f;
		l_glmRGBtoYUV[0][2] =  0.615f;
		l_glmRGBtoYUV[1][2] = -0.51499f;
		l_glmRGBtoYUV[2][2] = -0.10001f;
		l_glmRGBtoYUV[3][2] = 0.0f;
		l_glmRGBtoYUV[0][3] =  0.0f;
		l_glmRGBtoYUV[1][3] = 0.0f;
		l_glmRGBtoYUV[2][3] = 0.0f;
		l_glmRGBtoYUV[3][3] = 1.0f;
		TEST_EQUAL( l_mtxRGBtoYUV, l_glmRGBtoYUV );
		Logger::LogMessage( cuT( "	Matrix Inversion with CastorUtils :" ) );
		Logger::LogMessage( cuT( "		Normal :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0], l_mtxRGBtoYUV[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1], l_mtxRGBtoYUV[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2], l_mtxRGBtoYUV[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][3], l_mtxRGBtoYUV[1][3], l_mtxRGBtoYUV[2][3], l_mtxRGBtoYUV[3][3] );
		Matrix4x4r l_mtxYUVtoRGB( l_mtxRGBtoYUV.get_inverse() );
		Logger::LogMessage( cuT( "		Inverted :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0], l_mtxYUVtoRGB[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1], l_mtxYUVtoRGB[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2], l_mtxYUVtoRGB[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][3], l_mtxYUVtoRGB[1][3], l_mtxYUVtoRGB[2][3], l_mtxYUVtoRGB[3][3] );
		Logger::LogMessage( cuT( "	Matrix Inversion with GLM :" ) );
		Logger::LogMessage( cuT( "		Normal :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][0], l_glmRGBtoYUV[1][0], l_glmRGBtoYUV[2][0], l_glmRGBtoYUV[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][1], l_glmRGBtoYUV[1][1], l_glmRGBtoYUV[2][1], l_glmRGBtoYUV[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][2], l_glmRGBtoYUV[1][2], l_glmRGBtoYUV[2][2], l_glmRGBtoYUV[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][3], l_glmRGBtoYUV[1][3], l_glmRGBtoYUV[2][3], l_glmRGBtoYUV[3][3] );
		glm::mat4x4 l_glmYUVtoRGB( glm::inverse( l_glmRGBtoYUV ) );
		Logger::LogMessage( cuT( "		Inverted :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][0], l_glmYUVtoRGB[1][0], l_glmYUVtoRGB[2][0], l_glmYUVtoRGB[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][1], l_glmYUVtoRGB[1][1], l_glmYUVtoRGB[2][1], l_glmYUVtoRGB[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][2], l_glmYUVtoRGB[1][2], l_glmYUVtoRGB[2][2], l_glmYUVtoRGB[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][3], l_glmYUVtoRGB[1][3], l_glmYUVtoRGB[2][3], l_glmYUVtoRGB[3][3] );
		TEST_EQUAL( l_mtxYUVtoRGB, l_glmYUVtoRGB );
	}
}
