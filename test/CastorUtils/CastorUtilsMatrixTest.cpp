#include "CastorUtilsMatrixTest.hpp"

#include <CastorUtils/Math/TransformationMatrix.hpp>
#include <CastorUtils/Stream/StreamIndentManipulators.hpp>

#if defined( CASTOR_USE_GLM )
#	define GLM_FORCE_RADIANS
#	define GLM_FORCE_DEPTH_ZERO_TO_ONE
#	include <glm/glm.hpp>
#	include <glm/gtc/quaternion.hpp>
#	include <glm/gtc/matrix_transform.hpp>
#endif

#include <random>

namespace Testing
{
	//*********************************************************************************************

	namespace matrix = c3d::matrix;
	using c3d::Angle;
	using c3d::Logger;
	using c3d::Matrix4x4f;
	using c3d::Matrix4x4d;
	using c3d::Matrix3x3f;
	using c3d::Matrix3x3d;
	using c3d::Matrix2x2f;
	using c3d::Matrix2x2d;
	using c3d::Point3f;
	using c3d::Point3d;
	using c3d::Point4f;
	using c3d::Point4d;
	using c3d::Quaternion;
	using c3d::StringStream;

	//*********************************************************************************************

	CastorUtilsMatrixTest::CastorUtilsMatrixTest()
		: TestCase( "CastorUtilsMatrixTest" )
	{
	}

	void CastorUtilsMatrixTest::doRegisterTests()
	{
		doRegisterTest( "MatrixInversion", [this](){ MatrixInversion(); } );
		doRegisterTest( "TransformDecompose", [this](){ TransformDecompose(); } );

#if defined( CASTOR_USE_GLM )

		doRegisterTest( "MatrixInversionComparison", [this](){ MatrixInversionComparison(); } );
		doRegisterTest( "MatrixNormalComparison", [this](){ MatrixNormalComparison(); } );
		doRegisterTest( "MatrixMultiplicationComparison", [this](){ MatrixMultiplicationComparison(); } );
		doRegisterTest( "TransformationMatrixComparison", [this](){ TransformationMatrixComparison(); } );
		doRegisterTest( "ProjectionMatrixComparison", [this](){ ProjectionMatrixComparison(); } );

#endif
	}

	bool CastorUtilsMatrixTest::compare( Matrix3x3f const & lhs, Matrix3x3f const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix3x3d const & lhs, Matrix3x3d const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix4x4f const & lhs, Matrix4x4f const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix4x4d const & lhs, Matrix4x4d const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( c3d::Point3d const & lhs, c3d::Point3d const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( c3d::Point3f const & lhs, c3d::Point3f const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( c3d::Quaternion const & lhs, c3d::Quaternion const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

#if defined( CASTOR_USE_GLM )

	bool CastorUtilsMatrixTest::compare( Matrix4x4f const & lhs, glm::mat4x4 const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix4x4d const & lhs, glm::mat4x4 const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix3x3f const & lhs, glm::mat3x3 const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix3x3d const & lhs, glm::mat3x3 const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix2x2f const & lhs, glm::mat2x2 const & rhs )const
	{
		return Testing::compare( lhs, rhs );
	}

	bool CastorUtilsMatrixTest::compare( Matrix2x2d const & lhs, glm::mat2x2 const & rhs )const
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
		mtxRGBtoYUV[0][1] =  0.7152;
		mtxRGBtoYUV[0][2] =  0.0722;
		mtxRGBtoYUV[1][0] = -0.09991;
		mtxRGBtoYUV[1][1] = -0.33609;
		mtxRGBtoYUV[1][2] =  0.436;
		mtxRGBtoYUV[2][0] =  0.615;
		mtxRGBtoYUV[2][1] = -0.55861;
		mtxRGBtoYUV[2][2] = -0.05639;
		mtxYUVtoRGB = mtxRGBtoYUV.getInverse();
		CT_EQUAL( mtxRGBtoYUV, mtxYUVtoRGB.getInverse() );
	}

	void CastorUtilsMatrixTest::TransformDecompose()
	{
		for ( int i = 0; i < 100; ++i )
		{
			Matrix4x4d mtxBase;
			Point3d posBase;
			randomInit( posBase.ptr(), 3u );
			Point3d sclBase;
			randomInit( sclBase.ptr(), 3u );
			Point3d axsBase;
			randomInit( axsBase.ptr(), 3u );
			float degBase;
			randomInit( &degBase, 1u );
			Quaternion rotBase = Quaternion::fromAxisAngle( axsBase, Angle::fromDegrees( degBase ) );
			matrix::setTransform( mtxBase, posBase, sclBase, rotBase );
			Point3d posDecomp;
			Point3d sclDecomp;
			Quaternion rotDecomp;
			matrix::decompose( mtxBase, posDecomp, sclDecomp, rotDecomp );
			CT_EQUAL( posBase, posDecomp );
			CT_EQUAL( sclBase, sclDecomp );
			CT_EQUAL( rotBase, rotDecomp );
			Matrix4x4d mtxDecomp;
			matrix::setTransform( mtxDecomp, posDecomp, sclDecomp, rotDecomp );
			CT_EQUAL( mtxBase, mtxDecomp );
		}

	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixTest::MatrixInversionComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
			{
				Matrix4x4f mtx;
				glm::mat4 glm;
				randomInit( mtx.ptr(), &glm[0][0], 16 );
				CT_EQUAL( mtx, glm );
				Matrix4x4f mtxInv( mtx.getInverse() );
				glm::mat4 glmInv( glm::inverse( glm ) );
				CT_EQUAL( mtxInv, glmInv );
			}
			{
				Matrix3x3f mtx;
				glm::mat3 glm;
				randomInit( mtx.ptr(), &glm[0][0], 9 );
				CT_EQUAL( mtx, glm );
				Matrix3x3f mtxInv( mtx.getInverse() );
				glm::mat3 glmInv( glm::inverse( glm ) );
				CT_EQUAL( mtxInv, glmInv );
			}
			{
				Matrix2x2f mtx;
				glm::mat2 glm;
				randomInit( mtx.ptr(), &glm[0][0], 4 );
				CT_EQUAL( mtx, glm );
				Matrix2x2f mtxInv( mtx.getInverse() );
				glm::mat2 glmInv( glm::inverse( glm ) );
				CT_EQUAL( mtxInv, glmInv );
			}
		}
	}
	
	void CastorUtilsMatrixTest::MatrixNormalComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
			Matrix4x4f cuModel;
			glm::mat4 glmModel;
			randomInit( cuModel.ptr(), &glmModel[0][0], 16 );
			CT_EQUAL( cuModel, glmModel );
			Matrix3x3f cuNormal{ cuModel };
			glm::mat3 glmNormal{ glmModel };
			CT_EQUAL( cuNormal, glmNormal );
			Matrix3x3f mtxInv( cuNormal.getInverse() );
			glm::mat3 glmInv( glm::inverse( glmNormal ) );
			CT_EQUAL( mtxInv, glmInv );
			Matrix3x3f mtxTransp( mtxInv.getTransposed() );
			glm::mat3 glmTransp( glm::transpose( glmInv ) );
			CT_EQUAL( mtxTransp, glmTransp );
		}
	}

	void CastorUtilsMatrixTest::MatrixMultiplicationComparison()
	{
		for ( int i = 0; i < 10; ++i )
		{
			Matrix4x4f mtxA;
			glm::mat4 glmA;
			randomInit( mtxA.ptr(), &glmA[0][0], 16 );
			Matrix4x4f mtxB;
			glm::mat4 glmB;
			randomInit( mtxB.ptr(), &glmB[0][0], 16 );
			CT_EQUAL( mtxA, glmA );
			CT_EQUAL( mtxB, glmB );
			Matrix4x4f mtxC( mtxA * mtxB );
			Matrix4x4f mtxD( mtxB * mtxA );
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
		CT_ON("	Translate" );

		for ( uint32_t i = 0; i < 100; i += 1 )
		{
			auto r = float( i );
			Point3f pt( r, r, r );
			Matrix4x4f mtx( 1 );
			matrix::translate( mtx, pt );
			glm::vec3 vec( r, r, r );
			glm::mat4 mat = glm::translate( glm::identity< glm::mat4 >(), vec );
			CT_EQUAL( mtx, mat );
		}

		CT_ON("	Scale" );

		for ( uint32_t i = 0; i < 100; i += 1 )
		{
			auto r = float( i );
			Point3f pt( r, r, r );
			Matrix4x4f mtx( 1 );
			matrix::scale( mtx, pt );
			glm::vec3 vec( r, r, r );
			glm::mat4 mat = glm::scale( glm::identity< glm::mat4 >(), vec );
			CT_EQUAL( mtx, mat );
		}
	}

	void CastorUtilsMatrixTest::ProjectionMatrixComparison()
	{
		float left = 200.0f;
		float right = 1920.0f;
		float top = 100.0f;
		float bottom = 1080.0f;
		float nearZ = 1.0f;
		float farZ = 1000.0f;
		CT_ON("	Ortho RH" );
		{
			Matrix4x4f mtx( 1 );
			matrix::ortho( mtx, left, right, bottom, top, nearZ, farZ );
			glm::mat4 mat;
			mat = glm::ortho( left, right, bottom, top, nearZ, farZ );
			CT_EQUAL( mtx, mat );
		}
		CT_ON("	Frustum" );
		{
			Matrix4x4f mtx( 1 );
			matrix::frustum( mtx, left, right, bottom, top, nearZ, farZ );
			glm::mat4 mat;
			mat = glm::frustum( left, right, bottom, top, nearZ, farZ );
			CT_EQUAL( mtx, mat );
		}
		CT_ON("	Perspective" );
		{
			Angle fov{ 90.0_degrees };
			float aspect = 4.0f / 3.0f;
			Matrix4x4f mtx( 1 );
			matrix::perspective( mtx, fov, aspect, nearZ, farZ );
			glm::mat4 mat;
			mat = glm::perspective< float >( fov.radians(), aspect, nearZ, farZ );
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

	void CastorUtilsMatrixBench::MatrixMultiplicationsCastor()const
	{
		doNotOptimizeAway( m_mtx1 * m_mtx2 );
	}

	void CastorUtilsMatrixBench::MatrixInversionCastor()const
	{
		doNotOptimizeAway( m_mtx1.getInverse() );
	}

	void CastorUtilsMatrixBench::MatrixCopyCastor()
	{
		doNotOptimizeAway( m_mtx2 = m_mtx1 );
	}

#if defined( CASTOR_USE_GLM )

	void CastorUtilsMatrixBench::MatrixMultiplicationsGlm()const
	{
		doNotOptimizeAway( m_mtx1glm * m_mtx2glm );
	}
	void CastorUtilsMatrixBench::MatrixInversionGlm()const
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
