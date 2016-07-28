#include "CastorUtilsMatrixTest.hpp"

#include <Math/TransformationMatrix.hpp>
#include <Stream/StreamIndentManipulators.hpp>

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

	void CastorUtilsMatrixTest::DoRegisterTests()
	{
		//DoRegisterTest( "MatrixInversion", std::bind( &CastorUtilsMatrixTest::MatrixInversion, this ) );

#if defined( CASTOR_USE_GLM )

		DoRegisterTest( "MatrixInversionComparison", std::bind( &CastorUtilsMatrixTest::MatrixInversionComparison, this ) );
		DoRegisterTest( "MatrixMultiplicationComparison", std::bind( &CastorUtilsMatrixTest::MatrixMultiplicationComparison, this ) );
		DoRegisterTest( "TransformationMatrixComparison", std::bind( &CastorUtilsMatrixTest::TransformationMatrixComparison, this ) );
		DoRegisterTest( "ProjectionMatrixComparison", std::bind( &CastorUtilsMatrixTest::ProjectionMatrixComparison, this ) );

#endif
	}

	void CastorUtilsMatrixTest::MatrixInversion()
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
		CT_EQUAL( l_mtxRGBtoYUV, l_mtxYUVtoRGB.get_inverse() );
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
		CT_EQUAL( l_mtxRGBtoYUV, l_mtxYUVtoRGB.get_inverse() );
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixTest::MatrixInversionComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
			char l_msg[64] = { 0 };
			Matrix4x4r l_mtx;
			glm::mat4 l_glm;
			randomInit( l_mtx.ptr(), &l_glm[0][0], 16 );
			CT_EQUAL( l_mtx, l_glm );
			Matrix4x4r l_mtxInv( l_mtx.get_inverse() );
			glm::mat4 l_glmInv( glm::inverse( l_glm ) );
			CT_EQUAL( l_mtxInv, l_glmInv );
		}
	}

	void CastorUtilsMatrixTest::MatrixMultiplicationComparison()
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
			CT_EQUAL( l_mtxA, l_glmA );
			CT_EQUAL( l_mtxB, l_glmB );
			Matrix4x4r l_mtxC( l_mtxA * l_mtxB );
			Matrix4x4r l_mtxD( l_mtxB * l_mtxA );
			glm::mat4 l_glmC( l_glmA * l_glmB );
			glm::mat4 l_glmD( l_glmB * l_glmA );
			CT_EQUAL( l_mtxC, l_glmC );
			CT_EQUAL( l_mtxD, l_glmD );
		}
	}

	void CastorUtilsMatrixTest::TransformationMatrixComparison()
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
			CT_EQUAL( l_mtx, l_mat );
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
			CT_EQUAL( l_mtx, l_mat );
		}

	}

	void CastorUtilsMatrixTest::ProjectionMatrixComparison()
	{
		real l_left = 200.0f;
		real l_right = 1920.0f;
		real l_top = 100.0f;
		real l_bottom = 1080.0f;
		real l_near = 1.0f;
		real l_far = 1000.0f;
		Logger::LogInfo( cuT( "	Ortho RH" ) );
		{
			Matrix4x4r l_mtx( 1 );
			matrix::ortho( l_mtx, l_left, l_right, l_bottom, l_top, l_near, l_far );
			glm::mat4 l_mat;
			l_mat = glm::ortho( l_left, l_right, l_bottom, l_top, l_near, l_far );
			CT_EQUAL( l_mtx, l_mat );
		}
		Logger::LogInfo( cuT( "	Frustum" ) );
		{
			Matrix4x4r l_mtx( 1 );
			matrix::frustum( l_mtx, l_left, l_right, l_bottom, l_top, l_near, l_far );
			glm::mat4 l_mat;
			l_mat = glm::frustum( l_left, l_right, l_bottom, l_top, l_near, l_far );
			CT_EQUAL( l_mtx, l_mat );
		}
		Logger::LogInfo( cuT( "	Perspective" ) );
		{
			Angle l_fov( Angle::from_degrees( 90.0 ) );
			real l_aspect = 4.0f / 3.0f;
			Matrix4x4r l_mtx( 1 );
			matrix::perspective( l_mtx, l_fov, l_aspect, l_near, l_far );
			glm::mat4 l_mat;
			l_mat = glm::perspective< real >( l_fov.radians(), l_aspect, l_near, l_far );
			CT_EQUAL( l_mtx, l_mat );
		}
		Logger::LogInfo( cuT( "	Look at" ) );
		{
			Matrix4x4r l_mtx( 1 );
			matrix::look_at( l_mtx, Point3d( 0, 0, 0 ), Point3d( 0, 0, 1 ), Point3d( 0, 1, 0 ) );
			glm::mat4 l_mat;
			l_mat = glm::lookAt( glm::vec3( 0, 0, 0 ), glm::vec3( 0, 0, 1 ), glm::vec3( 0, 1, 0 ) );
			CT_EQUAL( l_mtx, l_mat );
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
