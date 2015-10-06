#include "CastorUtilsMatrixTest.hpp"

#include <Matrix.hpp>
#include <TransformationMatrix.hpp>
#include <StreamIndentManipulators.hpp>

#pragma warning( disable:4996 )

#if defined( CASTOR_USE_GLM )
#	define GLM_FORCE_RADIANS
#	include <glm/glm.hpp>
#	include <glm/gtc/quaternion.hpp>
#	include <glm/gtc/matrix_transform.hpp>
#endif

#include <random>

namespace Testing
{
	//*********************************************************************************************

	template< typename StreamT, typename T, uint32_t Columns, uint32_t Rows >
	inline StreamT & operator<<( StreamT & p_streamOut, Castor::Matrix< T, Columns, Rows > const & p_matrix )
	{
		auto l_precision = p_streamOut.precision( 10 );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				p_streamOut.width( 15 );
				p_streamOut << std::right << p_matrix[i][j];
			}

			p_streamOut << std::endl;
		}

		p_streamOut.precision( l_precision );
		return p_streamOut;
	}

	template< typename T, size_t Rows >
	Castor::String PrintMatrix( Castor::SquareMatrix< T, Rows > const & p_matrix )
	{
		Castor::StringStream l_stream;
		l_stream << p_matrix;
		return l_stream.str();
	}

	template< typename T >
	inline void randomInit( T * p_pData1, T * p_pData2, uint32_t size )
	{
		std::random_device l_generator;
		std::uniform_real_distribution< T > l_distribution( T( 0.0 ), T( 1.0 ) );

		for ( uint32_t i = 0; i < size; ++i )
		{
			p_pData1[i] = l_distribution( l_generator );
			p_pData2[i] = p_pData1[i];
		}
	}

	template< typename T >
	inline void randomInit( T * p_pData, uint32_t size )
	{
		std::random_device l_generator;
		std::uniform_real_distribution< T > l_distribution( T( 0.0 ), T( 1.0 ) );

		for ( uint32_t i = 0; i < size; ++i )
		{
			p_pData[i] = l_distribution( l_generator );
		}
	}

#if defined( CASTOR_USE_GLM )

	template< typename T >
	inline bool operator==( Castor::SquareMatrix< T, 4 > const & a, glm::mat4x4 const & b )
	{
		T l_epsilon = T( 0.0001 );
		return std::abs( a[0][0] - b[0][0] ) < l_epsilon
			&& std::abs( a[0][1] - b[0][1] ) < l_epsilon
			&& std::abs( a[0][2] - b[0][2] ) < l_epsilon
			&& std::abs( a[0][3] - b[0][3] ) < l_epsilon
			&& std::abs( a[1][0] - b[1][0] ) < l_epsilon
			&& std::abs( a[1][1] - b[1][1] ) < l_epsilon
			&& std::abs( a[1][2] - b[1][2] ) < l_epsilon
			&& std::abs( a[1][3] - b[1][3] ) < l_epsilon
			&& std::abs( a[2][0] - b[2][0] ) < l_epsilon
			&& std::abs( a[2][1] - b[2][1] ) < l_epsilon
			&& std::abs( a[2][2] - b[2][2] ) < l_epsilon
			&& std::abs( a[2][3] - b[2][3] ) < l_epsilon
			&& std::abs( a[3][0] - b[3][0] ) < l_epsilon
			&& std::abs( a[3][1] - b[3][1] ) < l_epsilon
			&& std::abs( a[3][2] - b[3][2] ) < l_epsilon
			&& std::abs( a[3][3] - b[3][3] ) < l_epsilon;
	}

	template< typename CharType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & p_stream, glm::mat4 const & p_mtx )
	{
		auto l_precision = p_stream.precision( 10 );

		for ( int i = 0; i < 4; ++i )
		{
			for ( int j = 0; j < 4; ++j )
			{
				p_stream.width( 15 );
				p_stream << std::right << p_mtx[i][j];
			}

			p_stream << std::endl;
		}

		p_stream.precision( l_precision );
		return p_stream;
	}

	template< typename CharType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & p_stream, glm::mat3 const & p_mtx )
	{
		auto l_precision = p_stream.precision( 10 );

		for ( int i = 0; i < 3; ++i )
		{
			for ( int j = 0; j < 3; ++j )
			{
				p_stream.width( 15 );
				p_stream << std::right << p_mtx[i][j];
			}

			p_stream << std::endl;
		}

		p_stream.precision( l_precision );
		return p_stream;
	}

	Castor::String PrintMatrix( glm::mat4 const & p_matrix )
	{
		Castor::StringStream l_stream;
		l_stream << p_matrix;
		return l_stream.str();
	}

	Castor::String PrintMatrix( glm::mat3 const & p_matrix )
	{
		Castor::StringStream l_stream;
		l_stream << p_matrix;
		return l_stream.str();
	}

#endif

	//*********************************************************************************************

	template<>
	inline std::string to_string< Castor::Matrix4x4f >( Castor::Matrix4x4f const & p_value )
	{
		std::stringstream l_stream;
		l_stream << std::endl;
		::operator<<( l_stream, p_value );
		return l_stream.str();
	}

	template<>
	inline std::string to_string< Castor::Matrix4x4d >( Castor::Matrix4x4d const & p_value )
	{
		std::stringstream l_stream;
		l_stream << std::endl;
		::operator<<( l_stream, p_value );
		return l_stream.str();
	}

	template<>
	inline std::string to_string< Castor::Matrix3x3f >( Castor::Matrix3x3f const & p_value )
	{
		std::stringstream l_stream;
		l_stream << std::endl;
		::operator<<( l_stream, p_value );
		return l_stream.str();
	}

	template<>
	inline std::string to_string< Castor::Matrix3x3d >( Castor::Matrix3x3d const & p_value )
	{
		std::stringstream l_stream;
		l_stream << std::endl;
		::operator<<( l_stream, p_value );
		return l_stream.str();
	}

#if defined( CASTOR_USE_GLM )

	template<>
	inline std::string to_string< glm::mat4x4 >( glm::mat4x4 const & p_value )
	{
		std::stringstream l_stream;
		l_stream << std::endl;
		l_stream << p_value;
		return l_stream.str();
	}

	template<>
	inline std::string to_string< glm::mat3x3 >( glm::mat3x3 const & p_value )
	{
		std::stringstream l_stream;
		l_stream << std::endl;
		l_stream << p_value;
		return l_stream.str();
	}

#endif

	//*********************************************************************************************

	using Castor::real;
	using Castor::Angle;
	using Castor::Logger;
	using Castor::matrix;
	using Castor::Matrix4x4f;
	using Castor::Matrix4x4r;
	using Castor::Matrix4x4d;
	using Castor::Matrix3x3f;
	using Castor::Matrix3x3r;
	using Castor::Matrix3x3d;
	using Castor::Point3f;
	using Castor::Point3r;
	using Castor::Point3d;
	using Castor::Point4f;
	using Castor::Point4r;
	using Castor::Point4d;
	using Castor::Quaternion;
	using Castor::StringStream;

	//*********************************************************************************************

	CastorUtilsMatrixTest::CastorUtilsMatrixTest()
		: TestCase( "CastorUtilsMatrixTest" )
	{
	}

	CastorUtilsMatrixTest::~CastorUtilsMatrixTest()
	{
	}

	void CastorUtilsMatrixTest::Execute( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		EXECUTE_TEST( CastorUtilsMatrixTest, MatrixInversion, p_errCount, p_testCount );
#if defined( CASTOR_USE_GLM )
		EXECUTE_TEST( CastorUtilsMatrixTest, MatrixInversionComparison, p_errCount, p_testCount );
		EXECUTE_TEST( CastorUtilsMatrixTest, MatrixMultiplicationComparison, p_errCount, p_testCount );
		EXECUTE_TEST( CastorUtilsMatrixTest, TransformationMatrixComparison, p_errCount, p_testCount );
		EXECUTE_TEST( CastorUtilsMatrixTest, ProjectionMatrixComparison, p_errCount, p_testCount );
		EXECUTE_TEST( CastorUtilsMatrixTest, QuaternionComparison, p_errCount, p_testCount );
#endif
	}

	void CastorUtilsMatrixTest::MatrixInversion( uint32_t & p_errCount, uint32_t & p_testCount )
	{
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
		Matrix3x3d l_mtxYUVtoRGB( l_mtxRGBtoYUV.get_inverse() );
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
		l_mtxYUVtoRGB = l_mtxRGBtoYUV.get_inverse();
		TEST_EQUAL( l_mtxRGBtoYUV, l_mtxYUVtoRGB.get_inverse() );
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixTest::MatrixInversionComparison( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		for ( int i = 0; i < 10; ++i )
		{
			char l_msg[64] = { 0 };
			Matrix4x4r l_mtx;
			glm::mat4 l_glm;
			randomInit( l_mtx.ptr(), &l_glm[0][0], 16 );
			TEST_EQUAL( l_mtx, l_glm );
			Matrix4x4r l_mtxInv( l_mtx.get_inverse() );
			glm::mat4 l_glmInv( glm::inverse( l_glm ) );
			TEST_EQUAL( l_mtxInv, l_glmInv );
		}
	}

	void CastorUtilsMatrixTest::MatrixMultiplicationComparison( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		for ( int i = 0; i < 10; ++i )
		{
			char l_msg[64] = { 0 };
			Matrix4x4r l_mtxA;
			glm::mat4 l_glmA;
			randomInit( l_mtxA.ptr(), &l_glmA[0][0], 16 );
			Matrix4x4r l_mtxB;
			glm::mat4 l_glmB;
			randomInit( l_mtxB.ptr(), &l_glmB[0][0], 16 );
			TEST_EQUAL( l_mtxA, l_glmA );
			TEST_EQUAL( l_mtxB, l_glmB );
			Matrix4x4r l_mtxC( l_mtxA * l_mtxB );
			Matrix4x4r l_mtxD( l_mtxB * l_mtxA );
			glm::mat4 l_glmC( l_glmA * l_glmB );
			glm::mat4 l_glmD( l_glmB * l_glmA );
			TEST_EQUAL( l_mtxC, l_glmC );
			TEST_EQUAL( l_mtxD, l_glmD );
		}
	}

	void CastorUtilsMatrixTest::TransformationMatrixComparison( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Logger::LogInfo( cuT( "	Translate" ) );

		for ( real r = 0; r < 100; r += 1 )
		{
			Point3r l_pt( r, r, r );
			Matrix4x4r l_mtx( 1 );
			matrix::translate( l_mtx, l_pt );
			glm::vec3 l_vec( r, r, r );
			glm::mat4 l_mat;
			l_mat = glm::translate( l_mat, l_vec );
			TEST_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Scale" ) );

		for ( real r = 0; r < 100; r += 1 )
		{
			Point3r l_pt( r, r, r );
			Matrix4x4r l_mtx( 1 );
			matrix::scale( l_mtx, l_pt );
			glm::vec3 l_vec( r, r, r );
			glm::mat4 l_mat;
			l_mat = glm::scale( l_mat, l_vec );
			TEST_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on X" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Angle::FromDegrees( r ), Point3r( 1, 0, 0 ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 1, 0, 0 ) );
			TEST_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Y" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Angle::FromDegrees( r ), Point3r( 0, 1, 0 ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 0, 1, 0 ) );
			TEST_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Z" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Angle::FromDegrees( r ), Point3r( 0, 0, 1 ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 0, 0, 1 ) );
			TEST_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on All" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Angle::FromDegrees( r ), Point3r( 1, 1, 1 ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 1, 1, 1 ) );
			TEST_EQUAL( l_mtx, l_mat );
		}
	}

	void CastorUtilsMatrixTest::ProjectionMatrixComparison( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		double l_left = 200.0;
		double l_right = 1920.0;
		double l_top = 100.0;
		double l_bottom = 1080.0;
		double l_near = 1.0;
		double l_far = 1000.0;
		Logger::LogInfo( cuT( "	Ortho RH" ) );
		{
			Matrix4x4r l_mtx( 1 );
			matrix::ortho( l_mtx, l_left, l_right, l_bottom, l_top, l_near, l_far );
			glm::mat4 l_mat;
			l_mat = glm::ortho( l_left, l_right, l_bottom, l_top, l_near, l_far );
			TEST_EQUAL( l_mtx, l_mat );
		}
		Logger::LogInfo( cuT( "	Frustum" ) );
		{
			Matrix4x4r l_mtx( 1 );
			matrix::frustum( l_mtx, l_left, l_right, l_bottom, l_top, l_near, l_far );
			glm::mat4 l_mat;
			l_mat = glm::frustum( l_left, l_right, l_bottom, l_top, l_near, l_far );
			TEST_EQUAL( l_mtx, l_mat );
		}
		Logger::LogInfo( cuT( "	Perspective" ) );
		{
			Angle l_fov( Angle::FromDegrees( 90.0 ) );
			double l_aspect = 4.0 / 3.0;
			Matrix4x4r l_mtx( 1 );
			matrix::perspective( l_mtx, l_fov, l_aspect, l_near, l_far );
			glm::mat4 l_mat;
			l_mat = glm::perspective( l_fov.Radians(), l_aspect, l_near, l_far );
			TEST_EQUAL( l_mtx, l_mat );
		}
		Logger::LogInfo( cuT( "	Look at" ) );
		{
			Matrix4x4r l_mtx( 1 );
			matrix::look_at( l_mtx, Point3d( 0, 0, 0 ), Point3d( 0, 0, 1 ), Point3d( 0, 1, 0 ) );
			glm::mat4 l_mat;
			l_mat = glm::lookAt( glm::vec3( 0, 0, 0 ), glm::vec3( 0, 0, 1 ), glm::vec3( 0, 1, 0 ) );
			TEST_EQUAL( l_mtx, l_mat );
		}
		Logger::LogInfo( cuT( "	Transform" ) );
		{
			Matrix4x4r l_mtx( 1 );
			Angle l_angle( Angle::FromDegrees( 45 ) );
			matrix::set_transform( l_mtx, Point3d( 0, 0, -100 ), Point3d( 0.3, 0.6, 1.0 ), Quaternion( Point3d( 0.0, 1.0, 0.0 ), l_angle ) );
			glm::mat4 l_mat( 1 );
			{
				// Compute transformation matrix with glm
				glm::mat4 l_rotation = glm::mat4_cast( glm::angleAxis( float( l_angle.Radians() ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
				glm::mat4 l_translation = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 0.0f, -100.0f ) );
				glm::mat4 l_scale = glm::scale( glm::mat4( 1.0f ), glm::vec3( 0.3f, 0.6f, 1.0f ) );
				l_mat = l_translation * l_rotation * l_scale;
			}
			TEST_EQUAL( l_mtx, l_mat );
		}
	}

	void CastorUtilsMatrixTest::QuaternionComparison( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Logger::LogInfo( cuT( "	Rotate on X" ) );
		for ( double r = 0; r < 360; r += 1 )
		{
			Quaternion l_quaternion( Point3d( 1, 0, 0 ), Angle::FromDegrees( r ) );
			glm::quat l_quat = glm::angleAxis( glm::radians( float( r ) ), glm::vec3( 1, 0, 0 ) );
			Matrix4x4d l_matrix;
			l_quaternion.ToRotationMatrix( l_matrix );
			glm::mat4 l_mat = glm::mat4_cast( l_quat );
			TEST_EQUAL( l_matrix, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Y" ) );
		for ( double r = 0; r < 360; r += 1 )
		{
				Quaternion l_quaternion( Point3r( 0, 1, 0 ), Angle::FromDegrees( r ) );
				glm::quat l_quat = glm::angleAxis( glm::radians( float( r ) ), glm::vec3( 0, 1, 0 ) );
				Matrix4x4d l_matrix;
				l_quaternion.ToRotationMatrix( l_matrix );
				glm::mat4 l_mat = glm::mat4_cast( l_quat );
				TEST_EQUAL( l_matrix, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Z" ) );
		for ( double r = 0; r < 360; r += 1 )
		{
				Quaternion l_quaternion( Point3r( 0, 0, 1 ), Angle::FromDegrees( r ) );
				glm::quat l_quat = glm::angleAxis( glm::radians( float( r ) ), glm::vec3( 0, 0, 1 ) );
				Matrix4x4d l_matrix;
				l_quaternion.ToRotationMatrix( l_matrix );
				glm::mat4 l_mat = glm::mat4_cast( l_quat );
				TEST_EQUAL( l_matrix, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on all" ) );
		for ( double r = 0; r < 360; r += 1 )
		{
				Quaternion l_quaternion( Angle::FromDegrees( r ), Angle::FromDegrees( r ), Angle::FromDegrees( r ) );
				glm::quat l_quat( glm::vec3( glm::radians( float( r ) ), glm::radians( float( r ) ), glm::radians( float( r ) ) ) );
				Matrix4x4d l_matrix;
				l_quaternion.ToRotationMatrix( l_matrix );
				glm::mat4 l_mat = glm::mat4_cast( l_quat );
				TEST_EQUAL( l_matrix, l_mat );
				TEST_EQUAL( l_quaternion.GetPitch().Radians(), glm::pitch( l_quat ) );
				TEST_EQUAL( l_quaternion.GetYaw().Radians(), glm::yaw( l_quat ) );
				TEST_EQUAL( l_quaternion.GetRoll().Radians(), glm::roll( l_quat ) );
		}

		Quaternion l_quaternionSrc( Point3r( 1, 1, 1 ), Angle::FromDegrees( 0 ) );
		Quaternion l_quaternionDst( Point3r( 1, 1, 1 ), Angle::FromDegrees( 100 ) );
		glm::quat l_quatSrc = glm::angleAxis( glm::radians( 0.0f ), glm::normalize( glm::vec3( 1, 1, 1 ) ) );
		glm::quat l_quatDst = glm::angleAxis( glm::radians( 100.0f ), glm::normalize( glm::vec3( 1, 1, 1 ) ) );

		Logger::LogInfo( cuT( "	Mix" ) );
		{
			for ( double r = 0; r < 100; r += 1 )
			{
				Quaternion l_quaternion = l_quaternionSrc.Mix( l_quaternionDst, r / 100 );
				glm::quat l_quat = glm::mix( l_quatSrc, l_quatDst, float( r / 100 ) );
				Matrix4x4d l_matrix;
				l_quaternion.ToRotationMatrix( l_matrix );
				glm::mat4 l_mat = glm::mat4_cast( l_quat );
				TEST_EQUAL( l_matrix, l_mat );
			}
		}

		Logger::LogInfo( cuT( "	Lerp" ) );
		{

			for ( double r = 0; r < 100; r += 1 )
			{
				Quaternion l_quaternion = l_quaternionSrc.Lerp( l_quaternionDst, r / 100 );
				glm::quat l_quat = glm::lerp( l_quatSrc, l_quatDst, float( r / 100 ) );
				Matrix4x4d l_matrix;
				l_quaternion.ToRotationMatrix( l_matrix );
				glm::mat4 l_mat = glm::mat4_cast( l_quat );
				TEST_EQUAL( l_matrix, l_mat );
			}
		}

		Logger::LogInfo( cuT( "	Slerp" ) );
		{
			for ( double r = 0; r < 100; r += 1 )
			{
				Quaternion l_quaternion = l_quaternionSrc.Slerp( l_quaternionDst, r / 100 );
				glm::quat l_quat = glm::slerp( l_quatSrc, l_quatDst, float( r / 100 ) );
				Matrix4x4d l_matrix;
				l_quaternion.ToRotationMatrix( l_matrix );
				glm::mat4 l_mat = glm::mat4_cast( l_quat );
				TEST_EQUAL( l_matrix, l_mat );
			}
		}
	}

#endif

	//*********************************************************************************************

	CastorUtilsMatrixBench::CastorUtilsMatrixBench()
		: BenchCase( "CastorUtilsMatrixBench" )
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

	CastorUtilsMatrixBench::~CastorUtilsMatrixBench()
	{
	}

	void CastorUtilsMatrixBench::Execute()
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
	}

	void CastorUtilsMatrixBench::MatrixMultiplicationsCastor()
	{
		DoNotOptimizeAway( m_mtx1 * m_mtx2 );
	}

	void CastorUtilsMatrixBench::MatrixInversionCastor()
	{
		DoNotOptimizeAway( m_mtx1.get_inverse() );
	}

	void CastorUtilsMatrixBench::MatrixCopyCastor()
	{
		DoNotOptimizeAway( m_mtx2 = m_mtx1 );
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixBench::MatrixMultiplicationsGlm()
	{
		DoNotOptimizeAway( m_mtx1glm * m_mtx2glm );
	}
	void CastorUtilsMatrixBench::MatrixInversionGlm()
	{
		DoNotOptimizeAway( glm::inverse( m_mtx1glm ) );
	}
	void CastorUtilsMatrixBench::MatrixCopyGlm()
	{
		DoNotOptimizeAway( m_mtx2glm = m_mtx1glm );
	}

#endif

	//*********************************************************************************************
}
