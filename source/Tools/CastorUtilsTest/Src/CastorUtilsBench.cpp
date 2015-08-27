#include "CastorUtilsBench.hpp"

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
#if defined( CASTOR_USE_GLM )
 	inline bool operator ==( Castor::Matrix4x4r const & a, glm::mat4x4 const & b )
	{
		return a[0][0] == b[0][0] && a[1][0] == b[1][0] &&	a[2][0] == b[2][0] &&	a[3][0] == b[3][0]
			   && a[0][1] == b[0][1] && a[1][1] == b[1][1] &&	a[2][1] == b[2][1] &&	a[3][1] == b[3][1]
			   && a[0][2] == b[0][2] && a[1][2] == b[1][2] &&	a[2][2] == b[2][2] &&	a[3][2] == b[3][2]
			   && a[0][3] == b[0][3] && a[1][3] == b[1][3] &&	a[2][3] == b[2][3] &&	a[3][3] == b[3][3];
	}

	inline std::ostream & operator <<( std::ostream & p_stream, glm::mat4 const & p_mtx )
	{
		for ( uint32_t i = 0; i < 4; i++ )
		{
			for ( uint32_t j = 0; j < 4; j++ )
			{
				p_stream << L"\t" << p_mtx[j][i];
			}

			p_stream << std::endl;
		}

		return p_stream;
	}
#endif
}

namespace Testing
{
	CastorUtilsBench::CastorUtilsBench()
		:	BenchCase( "CastorUtilsBench" )
		,	m_strIn( "STR : Bonjoir éêèàÉÊÈÀ" )
		,	m_wstrIn( L"STR : Bonjoir éêèàÉÊÈÀ" )
	{
		m_mtx1[0][0] =  0.299f;
		m_mtx1[1][0] =  0.587f;
		m_mtx1[2][0] =  0.114f;
		m_mtx1[3][0] = 0.0f;
		m_mtx1[0][1] = -0.14713f;
		m_mtx1[1][1] = -0.28886f;
		m_mtx1[2][1] =  0.436f;
		m_mtx1[3][1] = 0.0f;
		m_mtx1[0][2] =  0.615f;
		m_mtx1[1][2] = -0.51499f;
		m_mtx1[2][2] = -0.10001f;
		m_mtx1[3][2] = 0.0f;
		m_mtx1[0][3] =  0.0f;
		m_mtx1[1][3] = 0.0f;
		m_mtx1[2][3] = 0.0f;
		m_mtx1[3][3] = 1.0f;
#if defined( CASTOR_USE_GLM )
		m_mtx1glm[0][0] =  0.299f;
		m_mtx1glm[1][0] =  0.587f;
		m_mtx1glm[2][0] =  0.114f;
		m_mtx1glm[3][0] = 0.0f;
		m_mtx1glm[0][1] = -0.14713f;
		m_mtx1glm[1][1] = -0.28886f;
		m_mtx1glm[2][1] =  0.436f;
		m_mtx1glm[3][1] = 0.0f;
		m_mtx1glm[0][2] =  0.615f;
		m_mtx1glm[1][2] = -0.51499f;
		m_mtx1glm[2][2] = -0.10001f;
		m_mtx1glm[3][2] = 0.0f;
		m_mtx1glm[0][3] =  0.0f;
		m_mtx1glm[1][3] = 0.0f;
		m_mtx1glm[2][3] = 0.0f;
		m_mtx1glm[3][3] = 1.0f;
		randomInit( m_mtx2.ptr(), &m_mtx2glm[0][0], 16 );
#endif
	}

	CastorUtilsBench::~CastorUtilsBench()
	{
	}

	void CastorUtilsBench::Execute()
	{
		BENCHMARK( MatrixMultiplicationsCastor, NB_TESTS );
#if defined( CASTOR_USE_GLM )
		BENCHMARK( MatrixMultiplicationsGlm, NB_TESTS );
#endif
		BENCHMARK( MatrixInversionCastor, NB_TESTS );
#if defined( CASTOR_USE_GLM )
		BENCHMARK( MatrixInversionGlm, NB_TESTS );
#endif
		BENCHMARK( MatrixCopyCastor, NB_TESTS );
#if defined( CASTOR_USE_GLM )
		BENCHMARK( MatrixCopyGlm, NB_TESTS );
#endif
		BENCHMARK( StrToWStrUsingConvert, NB_TESTS );
		BENCHMARK( StrToWStrUsingWiden, NB_TESTS );
		BENCHMARK( WStrToStrUsingConvert, NB_TESTS );
		BENCHMARK( WStrToStrUsingNarrow, NB_TESTS );
	}

	void CastorUtilsBench::MatrixMultiplicationsCastor()
	{
		DoNotOptimizeAway( m_mtx1 * m_mtx2 );
	}
#if defined( CASTOR_USE_GLM )
	void CastorUtilsBench::MatrixMultiplicationsGlm()
	{
		DoNotOptimizeAway( m_mtx1glm * m_mtx2glm );
	}
#endif
	void CastorUtilsBench::MatrixInversionCastor()
	{
		DoNotOptimizeAway( m_mtx1.get_inverse() );
	}
#if defined( CASTOR_USE_GLM )
	void CastorUtilsBench::MatrixInversionGlm()
	{
		DoNotOptimizeAway( glm::inverse( m_mtx1glm ) );
	}
#endif
	void CastorUtilsBench::MatrixCopyCastor()
	{
		DoNotOptimizeAway( m_mtx2 = m_mtx1 );
	}
#if defined( CASTOR_USE_GLM )
	void CastorUtilsBench::MatrixCopyGlm()
	{
		DoNotOptimizeAway( m_mtx2glm = m_mtx1glm );
	}
#endif
	void CastorUtilsBench::StrToWStrUsingConvert()
	{
		convert( m_strIn, m_wstrOut );
	}

	void CastorUtilsBench::StrToWStrUsingWiden()
	{
		widen( m_strIn, m_wstrOut );
	}

	void CastorUtilsBench::WStrToStrUsingConvert()
	{
		convert( m_wstrIn, m_strOut );
	}

	void CastorUtilsBench::WStrToStrUsingNarrow()
	{
		narrow( m_wstrIn, m_strOut );
	}
}
