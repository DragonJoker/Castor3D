#include "CastorUtilsMatrixTest.hpp"

#include <CastorUtils/Math/TransformationMatrix.hpp>
#include <CastorUtils/Stream/StreamIndentManipulators.hpp>

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
	using castor::Quaternion;
	using castor::StringStream;

	//*********************************************************************************************

	CastorUtilsMatrixTest::CastorUtilsMatrixTest()
		: TestCase( "CastorUtilsMatrixTest" )
	{
	}

	CastorUtilsMatrixTest::~CastorUtilsMatrixTest()
	{
	}

	void CastorUtilsMatrixTest::doRegisterTests()
	{
		doRegisterTest( "MatrixInversion", std::bind( &CastorUtilsMatrixTest::MatrixInversion, this ) );

#if defined( CASTOR_USE_GLM )

		//doRegisterTest( "MatrixInversionComparison", std::bind( &CastorUtilsMatrixTest::MatrixInversionComparison, this ) );
		doRegisterTest( "MatrixMultiplicationComparison", std::bind( &CastorUtilsMatrixTest::MatrixMultiplicationComparison, this ) );
		doRegisterTest( "TransformationMatrixComparison", std::bind( &CastorUtilsMatrixTest::TransformationMatrixComparison, this ) );
		doRegisterTest( "ProjectionMatrixComparison", std::bind( &CastorUtilsMatrixTest::ProjectionMatrixComparison, this ) );

#endif
	}

	bool CastorUtilsMatrixTest::compare( Matrix3x3f const & lhs, Matrix3x3f const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix3x3d const & lhs, Matrix3x3d const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix4x4f const & lhs, Matrix4x4f const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix4x4d const & lhs, Matrix4x4d const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

#if defined( CASTOR_USE_GLM )

	bool CastorUtilsMatrixTest::compare( Matrix4x4f const & lhs, glm::mat4x4 const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix4x4d const & lhs, glm::mat4x4 const & rhs )
	{
		return Testing::compare( lhs, rhs );
	}

#endif

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
		Matrix3x3d mtxYUVtoRGB( mtxRGBtoYUV.getInverse() );
		CT_EQUAL( mtxRGBtoYUV, mtxYUVtoRGB.getInverse() );
		mtxRGBtoYUV[0][0] =  0.2126;
		mtxRGBtoYUV[1][0] =  0.7152;
		mtxRGBtoYUV[2][0] =  0.0722;
		mtxRGBtoYUV[0][1] = -0.09991;
		mtxRGBtoYUV[1][1] = -0.33609;
		mtxRGBtoYUV[2][1] =  0.436;
		mtxRGBtoYUV[0][2] =  0.615;
		mtxRGBtoYUV[1][2] = -0.55861;
		mtxRGBtoYUV[2][2] = -0.05639;
		mtxYUVtoRGB = mtxRGBtoYUV.getInverse();
		CT_EQUAL( mtxRGBtoYUV, mtxYUVtoRGB.getInverse() );
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixTest::MatrixInversionComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
			Matrix4x4f mtx;
			glm::mat4 glm;
			randomInit( mtx.ptr(), &glm[0][0], 16 );
			CT_EQUAL( mtx, glm );
			Matrix4x4f mtxInv( mtx.getInverse() );
			glm::mat4 glmInv( glm::inverse( glm ) );
			CT_EQUAL( mtxInv, glmInv );
		}
	}

	void CastorUtilsMatrixTest::MatrixMultiplicationComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
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
		Logger::logInfo( cuT( "	Translate" ) );

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

		Logger::logInfo( cuT( "	Scale" ) );

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
		Logger::logInfo( cuT( "	Ortho RH" ) );
		{
			Matrix4x4r mtx( 1 );
			matrix::ortho( mtx, left, right, bottom, top, near, far );
			glm::mat4 mat;
			mat = glm::ortho( left, right, bottom, top, near, far );
			CT_EQUAL( mtx, mat );
		}
		Logger::logInfo( cuT( "	Frustum" ) );
		{
			Matrix4x4r mtx( 1 );
			matrix::frustum( mtx, left, right, bottom, top, near, far );
			glm::mat4 mat;
			mat = glm::frustum( left, right, bottom, top, near, far );
			CT_EQUAL( mtx, mat );
		}
		Logger::logInfo( cuT( "	Perspective" ) );
		{
			Angle fov{ 90.0_degrees };
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
		doNotOptimizeAway( m_mtx1 * m_mtx2 );
	}

	void CastorUtilsMatrixBench::MatrixInversionCastor()
	{
		doNotOptimizeAway( m_mtx1.getInverse() );
	}

	void CastorUtilsMatrixBench::MatrixCopyCastor()
	{
		doNotOptimizeAway( m_mtx2 = m_mtx1 );
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixBench::MatrixMultiplicationsGlm()
	{
		doNotOptimizeAway( m_mtx1glm * m_mtx2glm );
	}
	void CastorUtilsMatrixBench::MatrixInversionGlm()
	{
		doNotOptimizeAway( glm::inverse( m_mtx1glm ) );
	}
	void CastorUtilsMatrixBench::MatrixCopyGlm()
	{
		doNotOptimizeAway( m_mtx2glm = m_mtx1glm );
	}

#endif

	//*********************************************************************************************
}
