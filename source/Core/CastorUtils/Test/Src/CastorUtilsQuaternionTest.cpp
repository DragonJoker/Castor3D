#include "CastorUtilsQuaternionTest.hpp"

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

	namespace matrix = Castor::matrix;
	using Castor::real;
	using Castor::Angle;
	using Castor::Logger;
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
	using Quaternion = Castor::QuaternionT< float >;
	using Castor::StringStream;

	//*********************************************************************************************

	CastorUtilsQuaternionTest::CastorUtilsQuaternionTest()
		: TestCase( "CastorUtilsQuaternionTest" )
	{
	}

	CastorUtilsQuaternionTest::~CastorUtilsQuaternionTest()
	{
	}

	void CastorUtilsQuaternionTest::DoRegisterTests()
	{
#if defined( CASTOR_USE_GLM )
#	if GLM_VERSION >= 95

		//DoRegisterTest( "QuaternionComparison", std::bind( &CastorUtilsQuaternionTest::QuaternionComparison, this ) );

#	endif

		DoRegisterTest( "TransformationMatrixComparison", std::bind( &CastorUtilsQuaternionTest::TransformationMatrixComparison, this ) );

#endif
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsQuaternionTest::TransformationMatrixComparison()
	{
		Logger::LogInfo( cuT( "	Rotate on X" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Quaternion( Point3r( 1, 0, 0 ), Angle::from_degrees( r ) ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 1, 0, 0 ) );
			CT_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Y" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Quaternion( Point3r( 0, 1, 0 ), Angle::from_degrees( r ) ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 0, 1, 0 ) );
			CT_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Z" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Quaternion( Point3r( 0, 0, 1 ), Angle::from_degrees( r ) ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 0, 0, 1 ) );
			CT_EQUAL( l_mtx, l_mat );
		}

		Logger::LogInfo( cuT( "	Rotate on All" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			matrix::rotate( l_mtx, Quaternion( Point3r( 1, 1, 1 ), Angle::from_degrees( r ) ) );
			glm::mat4 l_mat;
			l_mat = glm::rotate( l_mat, glm::radians( r ), glm::vec3( 1, 1, 1 ) );
			CT_EQUAL( l_mtx, l_mat );
		}

#	if GLM_VERSION >= 95

		Logger::LogInfo( cuT( "	Rotation" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Angle l_angle( Angle::from_degrees( r ) );
			glm::mat4 l_glm = glm::mat4_cast( glm::normalize( glm::angleAxis( float( l_angle.radians() ), glm::normalize( glm::vec3( 1.0f, 1.0f, 1.0f ) ) ) ) );
			Matrix4x4r l_mtx;
			Quaternion( Point3r( 1.0_r, 1.0_r, 1.0_r ), l_angle ).to_matrix( l_mtx );
			CT_EQUAL( l_mtx, l_glm );
		}

		Logger::LogInfo( cuT( "	Transform" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r l_mtx( 1 );
			Angle l_angle( Angle::from_degrees( r ) );
			matrix::set_transform( l_mtx, Point3r( r, r, -r ), Point3r( 0.3_r, 0.6_r, 1.0_r ), Quaternion( Point3r( 1.0_r, 1.0_r, 1.0_r ), l_angle ) );
			glm::mat4 l_mat( 1 );
			{
				// Compute transformation matrix with glm
				glm::mat4 l_rotation = glm::mat4_cast( glm::normalize( glm::angleAxis( float( l_angle.radians() ), glm::normalize( glm::vec3( 1.0f, 1.0f, 1.0f ) ) ) ) );
				glm::mat4 l_translation = glm::translate( glm::mat4( 1.0f ), glm::vec3( r, r, -r ) );
				glm::mat4 l_scale = glm::scale( glm::mat4( 1.0f ), glm::vec3( 0.3f, 0.6f, 1.0f ) );
				l_mat = l_translation;
				l_mat *= l_rotation;
				l_mat *= l_scale;
				CT_EQUAL( l_translation * l_rotation * l_scale, l_mat );
				l_mat = glm::translate( glm::mat4( 1.0f ), glm::vec3( r, r, -r ) );
				l_mat *= l_rotation;
				l_mat = glm::scale( l_mat, glm::vec3( 0.3f, 0.6f, 1.0f ) );
				CT_EQUAL( l_translation * l_rotation * l_scale, l_mat );
			}
			CT_EQUAL( l_mtx, l_mat );
		}

#	endif
	}

#	if GLM_VERSION >= 95

	void CastorUtilsQuaternionTest::QuaternionComparison()
	{
		Logger::LogInfo( cuT( "	Rotate on X" ) );

		for ( double r = 0; r < 360; r += 1 )
		{
			Quaternion l_quaternion( Angle::from_degrees( r ), Angle::from_degrees( 0 ), Angle::from_degrees( 0 ) );
			glm::quat l_quat = glm::angleAxis( glm::radians( float( r ) ), glm::vec3( 1, 0, 0 ) );
			CT_EQUAL( l_quaternion.quat.x, l_quat.x );
			CT_EQUAL( l_quaternion.quat.y, l_quat.y );
			CT_EQUAL( l_quaternion.quat.z, l_quat.z );
			CT_EQUAL( l_quaternion.quat.w, l_quat.w );
		}

		Logger::LogInfo( cuT( "	Rotate on Y" ) );

		for ( double r = 0; r < 360; r += 1 )
		{
			Quaternion l_quaternion( Angle::from_degrees( 0 ), Angle::from_degrees( r ), Angle::from_degrees( 0 ) );
			glm::quat l_quat = glm::angleAxis( glm::radians( float( r ) ), glm::vec3( 0, 1, 0 ) );
			CT_EQUAL( l_quaternion.quat.x, l_quat.x );
			CT_EQUAL( l_quaternion.quat.y, l_quat.y );
			CT_EQUAL( l_quaternion.quat.z, l_quat.z );
			CT_EQUAL( l_quaternion.quat.w, l_quat.w );
		}

		Logger::LogInfo( cuT( "	Rotate on Z" ) );

		for ( double r = 0; r < 360; r += 1 )
		{
			Quaternion l_quaternion( Angle::from_degrees( 0 ), Angle::from_degrees( 0 ), Angle::from_degrees( r ) );
			glm::quat l_quat = glm::angleAxis( glm::radians( float( r ) ), glm::vec3( 0, 0, 1 ) );
			CT_EQUAL( l_quaternion.quat.x, l_quat.x );
			CT_EQUAL( l_quaternion.quat.y, l_quat.y );
			CT_EQUAL( l_quaternion.quat.z, l_quat.z );
			CT_EQUAL( l_quaternion.quat.w, l_quat.w );
		}

		Logger::LogInfo( cuT( "	Rotate on all" ) );

		for ( double x = 0; x < 360; x += 1 )
		{
			//Logger::LogDebug( StringStream() << cuT( "Pitch: " ) << x << cuT( ", Yaw: " ) << y << cuT( ", Roll: " ) << z );
			Quaternion l_quaternion( Angle::from_degrees( x ), Angle::from_degrees( x ), Angle::from_degrees( x ) );
			glm::quat l_quat( glm::vec3( glm::radians( float( x ) ), glm::radians( float( x ) ), glm::radians( float( x ) ) ) );
			CT_EQUAL( l_quaternion.quat.x, l_quat.x );
			CT_EQUAL( l_quaternion.quat.y, l_quat.y );
			CT_EQUAL( l_quaternion.quat.z, l_quat.z );
			CT_EQUAL( l_quaternion.quat.w, l_quat.w );
			CT_EQUAL( l_quaternion.get_pitch().radians(), glm::pitch( l_quat ) );
			CT_EQUAL( l_quaternion.get_yaw().radians(), glm::yaw( l_quat ) );
			CT_EQUAL( l_quaternion.get_roll().radians(), glm::roll( l_quat ) );
		}

		Quaternion l_quaternionSrc( Point3r( 1, 1, 1 ), Angle::from_degrees( 0 ) );
		Quaternion l_quaternionDst( Point3r( 1, 1, 1 ), Angle::from_degrees( 100 ) );
		glm::quat l_quatSrc = glm::angleAxis( glm::radians( 0.0f ), glm::normalize( glm::vec3( 1, 1, 1 ) ) );
		glm::quat l_quatDst = glm::angleAxis( glm::radians( 100.0f ), glm::normalize( glm::vec3( 1, 1, 1 ) ) );

		Logger::LogInfo( cuT( "	Mix" ) );
		{
			for ( double r = 0; r < 100; r += 1 )
			{
				Quaternion l_quaternion = l_quaternionSrc.mix( l_quaternionDst, r / 100 );
				glm::quat l_quat = glm::mix( l_quatSrc, l_quatDst, float( r / 100 ) );
			}
		}

		Logger::LogInfo( cuT( "	Lerp" ) );
		{

			for ( double r = 0; r < 100; r += 1 )
			{
				Quaternion l_quaternion = l_quaternionSrc.lerp( l_quaternionDst, r / 100 );
				glm::quat l_quat = glm::lerp( l_quatSrc, l_quatDst, float( r / 100 ) );
			}
		}

		Logger::LogInfo( cuT( "	Slerp" ) );
		{
			for ( double r = 0; r < 100; r += 1 )
			{
				Quaternion l_quaternion = l_quaternionSrc.slerp( l_quaternionDst, r / 100 );
				glm::quat l_quat = glm::slerp( l_quatSrc, l_quatDst, float( r / 100 ) );
			}
		}

		Logger::LogInfo( cuT( "	LookAt comparison" ) );
		{
			for ( double x = 0; x < 360; x += 1 )
			{
				Quaternion l_quaternion( Angle::from_degrees( x ), Angle::from_degrees( x ), Angle::from_degrees( x ) );
				Point3r l_right, l_up, l_lookat;
				l_quaternion.to_axes( l_right, l_up, l_lookat );
				Castor::point::normalise( l_up );
				Castor::point::normalise( l_right );
				Castor::point::normalise( l_lookat );
				Matrix4x4r l_rotate{ 1.0_r };
				auto & l_col0 = l_rotate[0];
				auto & l_col1 = l_rotate[1];
				auto & l_col2 = l_rotate[2];
				l_col0[0] = l_right[0];
				l_col0[1] = l_up[0];
				l_col0[2] = l_lookat[0];
				l_col0[3] = 0.0_r;
				l_col1[0] = l_right[1];
				l_col1[1] = l_up[1];
				l_col1[2] = l_lookat[1];
				l_col1[3] = 0.0_r;
				l_col2[0] = l_right[2];
				l_col2[1] = l_up[2];
				l_col2[2] = l_lookat[2];
				l_col2[3] = 0.0_r;
				Matrix4x4r l_orientation;
				l_quaternion.to_matrix( l_orientation );
				CT_EQUAL( l_rotate, l_orientation );
			}
		}

		Logger::LogInfo( cuT( "	LookAt" ) );
		{
			for ( double x = 0; x < 360; x += 1 )
			{
				Quaternion l_quaternion( Angle::from_degrees( x ), Angle::from_degrees( x ), Angle::from_degrees( x ) );
				Point3r l_right, l_up, l_lookat;
				l_quaternion.to_axes( l_right, l_up, l_lookat );
				Castor::point::normalise( l_up );
				Castor::point::normalise( l_right );
				Castor::point::normalise( l_lookat );
				Matrix4x4r l_mtx( 1 );
				matrix::look_at( l_mtx, Point3r( x, x, x ), l_lookat, l_up );
				glm::mat4 l_mat;
				l_mat = glm::lookAt( glm::vec3( x, x, x ), glm::vec3( l_lookat[0], l_lookat[1], l_lookat[2] ), glm::vec3( l_up[0], l_up[1], l_up[2] ) );
				CT_EQUAL( l_mtx, l_mat );
			}
		}
	}

#	endif

#endif

	//*********************************************************************************************
}
