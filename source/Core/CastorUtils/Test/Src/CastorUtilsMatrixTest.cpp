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
		DoRegisterTest( "MatrixInversion", std::bind( &CastorUtilsMatrixTest::MatrixInversion, this ) );

#if defined( CASTOR_USE_GLM )

		//DoRegisterTest( "MatrixInversionComparison", std::bind( &CastorUtilsMatrixTest::MatrixInversionComparison, this ) );
		DoRegisterTest( "MatrixMultiplicationComparison", std::bind( &CastorUtilsMatrixTest::MatrixMultiplicationComparison, this ) );
		DoRegisterTest( "TransformationMatrixComparison", std::bind( &CastorUtilsMatrixTest::TransformationMatrixComparison, this ) );
		DoRegisterTest( "ProjectionMatrixComparison", std::bind( &CastorUtilsMatrixTest::ProjectionMatrixComparison, this ) );

#endif
	}

	void CastorUtilsMatrixTest::MatrixInversion()
	{
		Matrix3x3d mtxRGBtoYUV;
		mtxRGBtoYUV[0][0] =  0.299;
		mtxRGBtoYUV[1][0] =  0.587;
		mtxRGBtoYUV[2][0] =  0.114;
		mtxRGBtoYUV[0][1] = -0.14713;
		mtxRGBtoYUV[1][1] = -0.28886;
		mtxRGBtoYUV[2][1] =  0.436;
		mtxRGBtoYUV[0][2] =  0.615;
		mtxRGBtoYUV[1][2] = -0.51499;
		mtxRGBtoYUV[2][2] = -0.10001;
		Matrix3x3d mtxYUVtoRGB( mtxRGBtoYUV.get_inverse() );
		CT_EQUAL( mtxRGBtoYUV, mtxYUVtoRGB.get_inverse() );
		mtxRGBtoYUV[0][0] =  0.2126;
		mtxRGBtoYUV[1][0] =  0.7152;
		mtxRGBtoYUV[2][0] =  0.0722;
		mtxRGBtoYUV[0][1] = -0.09991;
		mtxRGBtoYUV[1][1] = -0.33609;
		mtxRGBtoYUV[2][1] =  0.436;
		mtxRGBtoYUV[0][2] =  0.615;
		mtxRGBtoYUV[1][2] = -0.55861;
		mtxRGBtoYUV[2][2] = -0.05639;
		mtxYUVtoRGB = mtxRGBtoYUV.get_inverse();
		CT_EQUAL( mtxRGBtoYUV, mtxYUVtoRGB.get_inverse() );
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixTest::MatrixInversionComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
			char msg[64] = { 0 };
			Matrix4x4f mtx;
			glm::mat4 glm;
			randomInit( mtx.ptr(), &glm[0][0], 16 );
			CT_EQUAL( mtx, glm );
			Matrix4x4f mtxInv( mtx.get_inverse() );
			glm::mat4 glmInv( glm::inverse( glm ) );
			CT_EQUAL( mtxInv, glmInv );
		}
	}

	void CastorUtilsMatrixTest::MatrixMultiplicationComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
			char msg[64] = { 0 };
			Matrix4x4r mtxA;
			glm::mat4 glmA;
			randomInit( mtxA.ptr(), &glmA[0][0], 16 );
			Matrix4x4r mtxB;
			glm::mat4 glmB;
			randomInit( mtxB.ptr(), &glmB[0][0], 16 );
			CT_EQUAL( mtxA, glmA );
			CT_EQUAL( mtxB, glmB );
			Matrix4x4r mtxC( mtxA * mtxB );
			Matrix4x4r mtxD( mtxB * mtxA );
			glm::mat4 glmC( glmA * glmB );
			glm::mat4 glmD( glmB * glmA );
			CT_EQUAL( mtxC, glmC );
			CT_EQUAL( mtxD, glmD );
			mtxA *= mtxB;
			glmA *= glmB;
			CT_EQUAL( mtxA, glmA );
		}
	}

	void CastorUtilsMatrixTest::TransformationMatrixComparison()
	{
		Logger::LogInfo( cuT( "	Translate" ) );

		for ( real r = 0; r < 100; r += 1 )
		{
			Point3r pt( r, r, r );
			Matrix4x4r mtx( 1 );
			matrix::translate( mtx, pt );
			glm::vec3 vec( r, r, r );
			glm::mat4 mat;
			mat = glm::translate( mat, vec );
			CT_EQUAL( mtx, mat );
		}

		Logger::LogInfo( cuT( "	Scale" ) );

		for ( real r = 0; r < 100; r += 1 )
		{
			Point3r pt( r, r, r );
			Matrix4x4r mtx( 1 );
			matrix::scale( mtx, pt );
			glm::vec3 vec( r, r, r );
			glm::mat4 mat;
			mat = glm::scale( mat, vec );
			CT_EQUAL( mtx, mat );
		}
	}

	void CastorUtilsMatrixTest::ProjectionMatrixComparison()
	{
		real left = 200.0f;
		real right = 1920.0f;
		real top = 100.0f;
		real bottom = 1080.0f;
		real near = 1.0f;
		real far = 1000.0f;
		Logger::LogInfo( cuT( "	Ortho RH" ) );
		{
			Matrix4x4r mtx( 1 );
			matrix::ortho( mtx, left, right, bottom, top, near, far );
			glm::mat4 mat;
			mat = glm::ortho( left, right, bottom, top, near, far );
			CT_EQUAL( mtx, mat );
		}
		Logger::LogInfo( cuT( "	Frustum" ) );
		{
			Matrix4x4r mtx( 1 );
			matrix::frustum( mtx, left, right, bottom, top, near, far );
			glm::mat4 mat;
			mat = glm::frustum( left, right, bottom, top, near, far );
			CT_EQUAL( mtx, mat );
		}
		Logger::LogInfo( cuT( "	Perspective" ) );
		{
			Angle fov( Angle::from_degrees( 90.0 ) );
			real aspect = 4.0f / 3.0f;
			Matrix4x4r mtx( 1 );
			matrix::perspective( mtx, fov, aspect, near, far );
			glm::mat4 mat;
			mat = glm::perspective< real >( fov.radians(), aspect, near, far );
			CT_EQUAL( mtx, mat );
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
