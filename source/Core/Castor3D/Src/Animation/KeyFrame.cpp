#include "KeyFrame.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename T, typename U >
		SquareMatrix< T, 4 > & rotate( SquareMatrix< T, 4 > & p_matrix, QuaternionT< U > const & p_quat )
		{
			SquareMatrix< T, 4 > l_rotate;
			auto const qxx( p_quat.quat.x * p_quat.quat.x );
			auto const qyy( p_quat.quat.y * p_quat.quat.y );
			auto const qzz( p_quat.quat.z * p_quat.quat.z );
			auto const qxz( p_quat.quat.x * p_quat.quat.z );
			auto const qxy( p_quat.quat.x * p_quat.quat.y );
			auto const qyz( p_quat.quat.y * p_quat.quat.z );
			auto const qwx( p_quat.quat.w * p_quat.quat.x );
			auto const qwy( p_quat.quat.w * p_quat.quat.y );
			auto const qwz( p_quat.quat.w * p_quat.quat.z );

			l_rotate[0][0] = T( 1 - 2 * ( qyy + qzz ) );
			l_rotate[0][1] = T( 2 * ( qxy - qwz ) );
			l_rotate[0][2] = T( 2 * ( qxz + qwy ) );
			l_rotate[0][3] = T( 0 );

			l_rotate[1][0] = T( 2 * ( qxy + qwz ) );
			l_rotate[1][1] = T( 1 - 2 * ( qxx + qzz ) );
			l_rotate[1][2] = T( 2 * ( qyz - qwx ) );
			l_rotate[1][3] = T( 0 );

			l_rotate[2][0] = T( 2 * ( qxz - qwy ) );
			l_rotate[2][1] = T( 2 * ( qyz + qwx ) );
			l_rotate[2][2] = T( 1 - 2 * ( qxx + qyy ) );
			l_rotate[3][3] = T( 0 );

			l_rotate[3][0] = T( 0 );
			l_rotate[3][1] = T( 0 );
			l_rotate[3][2] = T( 0 );
			l_rotate[3][3] = T( 1 );

			return p_matrix *= l_rotate;
		}
	}

	KeyFrame::KeyFrame( real p_timeIndex, Point3r const & p_translate, Quaternion const & p_rotate, Point3r const & p_scale )
		: m_timeIndex{ p_timeIndex }
		, m_transform{ 1.0_r }
	{
		matrix::translate( m_transform, p_translate );
		rotate( m_transform, p_rotate );
		matrix::scale( m_transform, p_scale );
	}

	KeyFrame::KeyFrame( real p_timeIndex, Castor::Matrix4x4r const & p_transform )
		: m_timeIndex{ p_timeIndex }
		, m_transform{ p_transform }
	{
	}
}
