/* See LICENSE file in root folder */
#ifndef ___CUT_TestPrerequisites___
#define ___CUT_TestPrerequisites___

#include <UnitTest.hpp>
#include <Benchmark.hpp>

#include <Math/SquareMatrix.hpp>

#include <random>

#if defined( CASTOR_USE_GLM )
#	define GLM_FORCE_RADIANS
#	include <glm/glm.hpp>
#	include <glm/gtc/quaternion.hpp>
#	include <glm/gtc/matrix_transform.hpp>
#endif

namespace Testing
{
	//*********************************************************************************************

	template< typename StreamT, typename T, uint32_t Columns, uint32_t Rows >
	inline StreamT & operator<<( StreamT & p_streamOut, castor::Matrix< T, Columns, Rows > const & p_matrix )
	{
		auto precision = p_streamOut.precision( 10 );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				p_streamOut.getWidth( 15 );
				p_streamOut << std::right << p_matrix[i][j];
			}

			p_streamOut << std::endl;
		}

		p_streamOut.precision( precision );
		return p_streamOut;
	}

	template< typename T, size_t Rows >
	castor::String PrintMatrix( castor::SquareMatrix< T, Rows > const & p_matrix )
	{
		castor::StringStream stream;
		stream << p_matrix;
		return stream.str();
	}

	template< typename T >
	inline void randomInit( T * p_pData1, T * p_pData2, uint32_t size )
	{
		std::random_device generator;
		std::uniform_real_distribution< T > distribution( T( 0.0 ), T( 1.0 ) );

		for ( uint32_t i = 0; i < size; ++i )
		{
			p_pData1[i] = distribution( generator );
			p_pData2[i] = p_pData1[i];
		}
	}

	template< typename T >
	inline void randomInit( T * p_pData, uint32_t size )
	{
		std::random_device generator;
		std::uniform_real_distribution< T > distribution( T( 0.0 ), T( 1.0 ) );

		for ( uint32_t i = 0; i < size; ++i )
		{
			p_pData[i] = distribution( generator );
		}
	}

	template< typename T >
	inline bool compare( castor::SquareMatrix< T, 4 > const & a, castor::SquareMatrix< T, 4 > const & b )
	{
		double epsilon = 0.001;
		return std::abs( a[0][0] - b[0][0] ) < epsilon
			&& std::abs( a[0][1] - b[0][1] ) < epsilon
			&& std::abs( a[0][2] - b[0][2] ) < epsilon
			&& std::abs( a[0][3] - b[0][3] ) < epsilon
			&& std::abs( a[1][0] - b[1][0] ) < epsilon
			&& std::abs( a[1][1] - b[1][1] ) < epsilon
			&& std::abs( a[1][2] - b[1][2] ) < epsilon
			&& std::abs( a[1][3] - b[1][3] ) < epsilon
			&& std::abs( a[2][0] - b[2][0] ) < epsilon
			&& std::abs( a[2][1] - b[2][1] ) < epsilon
			&& std::abs( a[2][2] - b[2][2] ) < epsilon
			&& std::abs( a[2][3] - b[2][3] ) < epsilon
			&& std::abs( a[3][0] - b[3][0] ) < epsilon
			&& std::abs( a[3][1] - b[3][1] ) < epsilon
			&& std::abs( a[3][2] - b[3][2] ) < epsilon
			&& std::abs( a[3][3] - b[3][3] ) < epsilon;
	}

	template< typename T >
	inline bool compare( castor::SquareMatrix< T, 3 > const & a, castor::SquareMatrix< T, 3 > const & b )
	{
		float epsilon = 0.001f;
		return std::abs( a[0][0] - b[0][0] ) < epsilon
			&& std::abs( a[0][1] - b[0][1] ) < epsilon
			&& std::abs( a[0][2] - b[0][2] ) < epsilon
			&& std::abs( a[1][0] - b[1][0] ) < epsilon
			&& std::abs( a[1][1] - b[1][1] ) < epsilon
			&& std::abs( a[1][2] - b[1][2] ) < epsilon
			&& std::abs( a[2][0] - b[2][0] ) < epsilon
			&& std::abs( a[2][1] - b[2][1] ) < epsilon
			&& std::abs( a[2][2] - b[2][2] ) < epsilon;
	}

#if defined( CASTOR_USE_GLM )

	template< typename T >
	inline bool compare( castor::SquareMatrix< T, 4 > const & a, glm::mat4x4 const & b )
	{
		T epsilon = T( 0.001 );
		return std::abs( a[0][0] - b[0][0] ) < epsilon
			&& std::abs( a[0][1] - b[0][1] ) < epsilon
			&& std::abs( a[0][2] - b[0][2] ) < epsilon
			&& std::abs( a[0][3] - b[0][3] ) < epsilon
			&& std::abs( a[1][0] - b[1][0] ) < epsilon
			&& std::abs( a[1][1] - b[1][1] ) < epsilon
			&& std::abs( a[1][2] - b[1][2] ) < epsilon
			&& std::abs( a[1][3] - b[1][3] ) < epsilon
			&& std::abs( a[2][0] - b[2][0] ) < epsilon
			&& std::abs( a[2][1] - b[2][1] ) < epsilon
			&& std::abs( a[2][2] - b[2][2] ) < epsilon
			&& std::abs( a[2][3] - b[2][3] ) < epsilon
			&& std::abs( a[3][0] - b[3][0] ) < epsilon
			&& std::abs( a[3][1] - b[3][1] ) < epsilon
			&& std::abs( a[3][2] - b[3][2] ) < epsilon
			&& std::abs( a[3][3] - b[3][3] ) < epsilon;
	}

	template< typename CharType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & p_stream, glm::mat4 const & p_mtx )
	{
		auto precision = p_stream.precision( 10 );

		for ( int i = 0; i < 4; ++i )
		{
			for ( int j = 0; j < 4; ++j )
			{
				p_stream.width( 15 );
				p_stream << std::right << p_mtx[i][j];
			}

			p_stream << std::endl;
		}

		p_stream.precision( precision );
		return p_stream;
	}

	template< typename CharType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & p_stream, glm::mat3 const & p_mtx )
	{
		auto precision = p_stream.precision( 10 );

		for ( int i = 0; i < 3; ++i )
		{
			for ( int j = 0; j < 3; ++j )
			{
				p_stream.width( 15 );
				p_stream << std::right << p_mtx[i][j];
			}

			p_stream << std::endl;
		}

		p_stream.precision( precision );
		return p_stream;
	}

	inline castor::String PrintMatrix( glm::mat4 const & p_matrix )
	{
		castor::StringStream stream;
		stream << p_matrix;
		return stream.str();
	}

	inline castor::String PrintMatrix( glm::mat3 const & p_matrix )
	{
		castor::StringStream stream;
		stream << p_matrix;
		return stream.str();
	}

#endif

	//*********************************************************************************************

	template<>
	inline std::string toString< castor::Matrix4x4f >( castor::Matrix4x4f const & p_value )
	{
		std::stringstream stream;
		stream << std::endl;
		castor::operator<<( stream, p_value );
		return stream.str();
	}

	template<>
	inline std::string toString< castor::Matrix4x4d >( castor::Matrix4x4d const & p_value )
	{
		std::stringstream stream;
		stream << std::endl;
		castor::operator<<( stream, p_value );
		return stream.str();
	}

	template<>
	inline std::string toString< castor::Matrix3x3f >( castor::Matrix3x3f const & p_value )
	{
		std::stringstream stream;
		stream << std::endl;
		castor::operator<<( stream, p_value );
		return stream.str();
	}

	template<>
	inline std::string toString< castor::Matrix3x3d >( castor::Matrix3x3d const & p_value )
	{
		std::stringstream stream;
		stream << std::endl;
		castor::operator<<( stream, p_value );
		return stream.str();
	}

#if defined( CASTOR_USE_GLM )

	template<>
	inline std::string toString< glm::mat4x4 >( glm::mat4x4 const & p_value )
	{
		std::stringstream stream;
		stream << std::endl;
		stream << p_value;
		return stream.str();
	}

	template<>
	inline std::string toString< glm::mat3x3 >( glm::mat3x3 const & p_value )
	{
		std::stringstream stream;
		stream << std::endl;
		stream << p_value;
		return stream.str();
	}

#endif

}

#endif
