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

	namespace matrix = castor::matrix;
	using castor::real;
	using castor::Angle;
	using castor::Logger;
	using castor::Matrix4x4f;
	using castor::Matrix4x4r;
	using castor::Matrix4x4d;
	using castor::Matrix3x3f;
	using castor::Matrix3x3r;
	using castor::Matrix3x3d;
	using castor::Point3f;
	using castor::Point3r;
	using castor::Point3d;
	using castor::Point4f;
	using castor::Point4r;
	using castor::Point4d;
	using Quaternion = castor::QuaternionT< float >;
	using castor::StringStream;

	//*********************************************************************************************

	CastorUtilsQuaternionTest::CastorUtilsQuaternionTest()
		: TestCase( "CastorUtilsQuaternionTest" )
	{
	}

	CastorUtilsQuaternionTest::~CastorUtilsQuaternionTest()
	{
	}

	void CastorUtilsQuaternionTest::doRegisterTests()
	{
#if defined( CASTOR_USE_GLM )

		doRegisterTest( "TransformationMatrixComparison", std::bind( &CastorUtilsQuaternionTest::TransformationMatrixComparison, this ) );

#endif
	}

#if defined( CASTOR_USE_GLM )

	bool CastorUtilsQuaternionTest::compare( Matrix4x4f const & lhs, glm::mat4x4 const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsQuaternionTest::compare( Matrix4x4d const & lhs, glm::mat4x4 const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

	void CastorUtilsQuaternionTest::TransformationMatrixComparison()
	{
		Logger::logInfo( cuT( "	Rotate on X" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::fromAxisAngle( Point3r( 1, 0, 0 ), Angle::fromDegrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 1, 0, 0 ) );
			CT_EQUAL( mtx, mat );
		}

		Logger::logInfo( cuT( "	Rotate on Y" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::fromAxisAngle( Point3r( 0, 1, 0 ), Angle::fromDegrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 0, 1, 0 ) );
			CT_EQUAL( mtx, mat );
		}

		Logger::logInfo( cuT( "	Rotate on Z" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::fromAxisAngle( Point3r( 0, 0, 1 ), Angle::fromDegrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 0, 0, 1 ) );
			CT_EQUAL( mtx, mat );
		}

		Logger::logInfo( cuT( "	Rotate on All" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			matrix::rotate( mtx, Quaternion::fromAxisAngle( Point3r( 1, 1, 1 ), Angle::fromDegrees( r ) ) );
			glm::mat4 mat;
			mat = glm::rotate( mat, glm::radians( r ), glm::vec3( 1, 1, 1 ) );
			CT_EQUAL( mtx, mat );
		}

#	if GLM_VERSION >= 95

		Logger::logInfo( cuT( "	Rotation" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Angle angle( Angle::fromDegrees( r ) );
			glm::mat4 glm = glm::mat4_cast( glm::normalize( glm::angleAxis( float( angle.radians() ), glm::normalize( glm::vec3( 1.0f, 1.0f, 1.0f ) ) ) ) );
			Matrix4x4r mtx;
			Quaternion::fromAxisAngle( Point3r( 1.0_r, 1.0_r, 1.0_r ), angle ).toMatrix( mtx );
			CT_EQUAL( mtx, glm );
		}

		Logger::logInfo( cuT( "	Transform" ) );

		for ( real r = 0; r < 360; r += 1 )
		{
			Matrix4x4r mtx( 1 );
			Angle angle( Angle::fromDegrees( r ) );
			matrix::setTransform( mtx, Point3r( r, r, -r ), Point3r( 0.3_r, 0.6_r, 1.0_r ), Quaternion::fromAxisAngle( Point3r( 1.0_r, 1.0_r, 1.0_r ), angle ) );
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
