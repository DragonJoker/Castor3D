/* See LICENSE file in root folder */
#ifndef ___CUT_TestPrerequisites___
#define ___CUT_TestPrerequisites___

#include <CastorTest/UnitTest.hpp>
#include <CastorTest/Benchmark.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <random>

#if defined( CASTOR_USE_GLM )
#	define GLM_FORCE_RADIANS
#	pragma warning( push )
#	pragma warning( disable:4201 )
#	pragma warning( disable:4464 )
#	pragma warning( disable:5054 )
#	pragma warning( disable:5214 )
#	include <glm/glm.hpp>
#	include <glm/gtc/quaternion.hpp>
#	include <glm/gtc/matrix_transform.hpp>
#	pragma warning( pop )
#endif

namespace Testing
{
	//*********************************************************************************************

	template< typename T >
	inline void randomInit( T * p_pData1, T * p_pData2, uint32_t size )
	{
		static std::random_device generator;
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
		static std::random_device generator;
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

	//*********************************************************************************************

	template< typename ValueT, uint32_t ColumnsT, uint32_t RowsT >
	struct Stringifier< castor::Matrix< ValueT, ColumnsT, RowsT > >
	{
		static std::string get( castor::Matrix< ValueT, ColumnsT, RowsT > const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( uint32_t i = 0; i < ColumnsT; i++ )
			{
				for ( uint32_t j = 0; j < RowsT; j++ )
				{
					stream << std::setw( 15 ) << std::right << value[i][j];
				}

				stream << std::endl;
			}

			return stream.str();
		}
	};

	template< typename ValueT, uint32_t CountT >
	struct Stringifier< castor::SquareMatrix< ValueT, CountT > >
	{
		static std::string get( castor::SquareMatrix< ValueT, CountT > const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( uint32_t i = 0; i < CountT; i++ )
			{
				for ( uint32_t j = 0; j < CountT; j++ )
				{
					stream << std::setw( 15 ) << std::right << value[i][j];
				}

				stream << std::endl;
			}

			return stream.str();
		}
	};
	
	template< typename ValueT, uint32_t CountT >
	struct Stringifier< castor::Point< ValueT, CountT > >
	{
		static std::string get( castor::Point< ValueT, CountT > const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( uint32_t i = 0; i < CountT; i++ )
			{
				stream << std::setw( 15 ) << std::right << value[i];
			}

			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< castor::AngleT< ValueT > >
	{
		static std::string get( castor::AngleT< ValueT > const & value )
		{
			std::stringstream stream;
			stream << value.degrees();
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< castor::ChangeTracked< ValueT > >
	{
		static std::string get( castor::ChangeTracked< ValueT > const & value )
		{
			std::stringstream stream;
			stream << toString( *value );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< castor::ArrayView< ValueT > >
	{
		static std::string get( castor::ArrayView< ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

#if defined( CASTOR_USE_GLM )

	//*********************************************************************************************

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

	template< typename T >
	inline bool compare( castor::SquareMatrix< T, 3 > const & a, glm::mat3x3 const & b )
	{
		T epsilon = T( 0.001 );
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

	template< typename T >
	inline bool compare( castor::SquareMatrix< T, 2 > const & a, glm::mat2x2 const & b )
	{
		T epsilon = T( 0.001 );
		return std::abs( a[0][0] - b[0][0] ) < epsilon
			&& std::abs( a[0][1] - b[0][1] ) < epsilon
			&& std::abs( a[1][0] - b[1][0] ) < epsilon
			&& std::abs( a[1][1] - b[1][1] ) < epsilon;
	}

	//*********************************************************************************************

	template<>
	struct Stringifier< glm::mat4 >
	{
		static std::string get( glm::mat4 const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( int i = 0; i < 4; ++i )
			{
				for ( int j = 0; j < 4; ++j )
				{
					stream << std::setw( 15 ) << std::right << value[i][j];
				}

				stream << std::endl;
			}

			return stream.str();
		}
	};

	template<>
	struct Stringifier< glm::mat3 >
	{
		static std::string get( glm::mat3 const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( int i = 0; i < 3; ++i )
			{
				for ( int j = 0; j < 3; ++j )
				{
					stream << std::setw( 15 ) << std::right << value[i][j];
				}

				stream << std::endl;
			}

			return stream.str();
		}
	};

	template<>
	struct Stringifier< glm::mat2 >
	{
		static std::string get( glm::mat2 const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( int i = 0; i < 2; ++i )
			{
				for ( int j = 0; j < 2; ++j )
				{
					stream << std::setw( 15 ) << std::right << value[i][j];
				}

				stream << std::endl;
			}

			return stream.str();
		}
	};

#endif

}

#endif
