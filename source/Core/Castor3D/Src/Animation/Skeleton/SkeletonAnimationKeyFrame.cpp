#include "SkeletonAnimationKeyFrame.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< typename T, typename U >
		SquareMatrix< T, 4 > & doRotate( SquareMatrix< T, 4 > & p_matrix, QuaternionT< U > const & p_quat )
		{
			SquareMatrix< T, 4 > rotate;
			auto const qxx( p_quat.quat.x * p_quat.quat.x );
			auto const qyy( p_quat.quat.y * p_quat.quat.y );
			auto const qzz( p_quat.quat.z * p_quat.quat.z );
			auto const qxz( p_quat.quat.x * p_quat.quat.z );
			auto const qxy( p_quat.quat.x * p_quat.quat.y );
			auto const qyz( p_quat.quat.y * p_quat.quat.z );
			auto const qwx( p_quat.quat.w * p_quat.quat.x );
			auto const qwy( p_quat.quat.w * p_quat.quat.y );
			auto const qwz( p_quat.quat.w * p_quat.quat.z );

			rotate[0][0] = T( 1 - 2 * ( qyy + qzz ) );
			rotate[0][1] = T( 2 * ( qxy - qwz ) );
			rotate[0][2] = T( 2 * ( qxz + qwy ) );
			rotate[0][3] = T( 0 );

			rotate[1][0] = T( 2 * ( qxy + qwz ) );
			rotate[1][1] = T( 1 - 2 * ( qxx + qzz ) );
			rotate[1][2] = T( 2 * ( qyz - qwx ) );
			rotate[1][3] = T( 0 );

			rotate[2][0] = T( 2 * ( qxz - qwy ) );
			rotate[2][1] = T( 2 * ( qyz + qwx ) );
			rotate[2][2] = T( 1 - 2 * ( qxx + qyy ) );
			rotate[3][3] = T( 0 );

			rotate[3][0] = T( 0 );
			rotate[3][1] = T( 0 );
			rotate[3][2] = T( 0 );
			rotate[3][3] = T( 1 );

			return p_matrix *= rotate;
		}
	}

	SkeletonAnimationKeyFrame::SkeletonAnimationKeyFrame( Skeleton & skeleton
		, Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< Skeleton >{ skeleton }
	{
	}

	void SkeletonAnimationKeyFrame::addAnimationObject( SkeletonAnimationObject const & object
		, Point3r const & translate
		, Quaternion const & rotate
		, Point3r const & scale )
	{
		Matrix4x4r transform{ 1.0_r };
		matrix::translate( transform, translate );
		doRotate( transform, rotate );
		matrix::scale( transform, scale );
		addAnimationObject( object, transform );
	}

	void SkeletonAnimationKeyFrame::addAnimationObject( SkeletonAnimationObject const & object
		, castor::Matrix4x4r const & transform )
	{
		m_keyFrames.emplace( &object, transform );
	}
}
