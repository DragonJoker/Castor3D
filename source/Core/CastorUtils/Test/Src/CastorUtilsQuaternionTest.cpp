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

		DoRegisterTest( "TransformationMatrixComparison", std::bind( &CastorUtilsQuaternionTest::TransformationMatrixComparison, this ) );

#endif
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsQuaternionTest::TransformationMatrixComparison()
	{
		Logger::LogInfo( cuT( "	Rotate on X" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::from_axis_angle( Point3r( 1, 0, 0 ), Angle::from_degrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 1, 0, 0 ) );
			CT_EQUAL( mtx, mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Y" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::from_axis_angle( Point3r( 0, 1, 0 ), Angle::from_degrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 0, 1, 0 ) );
			CT_EQUAL( mtx, mat );
		}

		Logger::LogInfo( cuT( "	Rotate on Z" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::from_axis_angle( Point3r( 0, 0, 1 ), Angle::from_degrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 0, 0, 1 ) );
			CT_EQUAL( mtx, mat );
		}

		Logger::LogInfo( cuT( "	Rotate on All" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::from_axis_angle( Point3r( 1, 1, 1 ), Angle::from_degrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 1, 1, 1 ) );
			CT_EQUAL( mtx, mat );
		}

#	if GLM_VERSION >= 95

		Logger::LogInfo( cuT( "	Rotation" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Angle angle( Angle::from_degrees( r ) );
			glm::mat4 glm = glm::mat4_cast( glm::normalize( glm::angleAxis( float( angle.radians() ), glm::normalize( glm::vec3( 1.0f, 1.0f, 1.0f ) ) ) ) );
			Matrix4x4r mtx;
			Quaternion::from_axis_angle( Point3r( 1.0_r, 1.0_r, 1.0_r ), angle ).to_matrix( mtx );
			CT_EQUAL( mtx, glm );
		}

		Logger::LogInfo( cuT( "	Transform" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			Angle angle( Angle::from_degrees( r ) );
			matrix::set_transform( mtx, Point3r( r, r, -r ), Point3r( 0.3_r, 0.6_r, 1.0_r ), Quaternion::from_axis_angle( Point3r( 1.0_r, 1.0_r, 1.0_r ), angle ) );
			glm::mat4 mat( 1 );
			{
				// Compute transformation matrix with glm
				glm::mat4 rotation = glm::mat4_cast( glm::normalize( glm::angleAxis( float( angle.radians() ), glm::normalize( glm::vec3( 1.0f, 1.0f, 1.0f ) ) ) ) );
				glm::mat4 translation = glm::translate( glm::mat4( 1.0f ), glm::vec3( r, r, -r ) );
				glm::mat4 scale = glm::scale( glm::mat4( 1.0f ), glm::vec3( 0.3f, 0.6f, 1.0f ) );
				mat = translation;
				mat *= rotation;
				mat *= scale;
				CT_EQUAL( translation * rotation * scale, mat );
				mat = glm::translate( glm::mat4( 1.0f ), glm::vec3( r, r, -r ) );
				mat *= rotation;
				mat = glm::scale( mat, glm::vec3( 0.3f, 0.6f, 1.0f ) );
				CT_EQUAL( translation * rotation * scale, mat );
			}
			CT_EQUAL( mtx, mat );
		}

#	endif
	}

#endif

	//*********************************************************************************************
}
