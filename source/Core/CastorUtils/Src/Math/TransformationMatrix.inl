#include "Quaternion.hpp"

namespace castor
{
	namespace matrix
	{
		template< typename T, typename U >
		SquareMatrix< T, 4 > & rotate( SquareMatrix< T, 4 > & p_matrix, QuaternionT< U > const & p_quat )
		{
			SquareMatrix< T, 4 > rotate;
			return p_matrix *= setRotate( rotate, p_quat );
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & setRotate( SquareMatrix< T, 4 > & p_matrix, QuaternionT< U > const & p_quat )
		{
			auto const qxx( p_quat.quat.x * p_quat.quat.x );
			auto const qyy( p_quat.quat.y * p_quat.quat.y );
			auto const qzz( p_quat.quat.z * p_quat.quat.z );
			auto const qxz( p_quat.quat.x * p_quat.quat.z );
			auto const qxy( p_quat.quat.x * p_quat.quat.y );
			auto const qyz( p_quat.quat.y * p_quat.quat.z );
			auto const qwx( p_quat.quat.w * p_quat.quat.x );
			auto const qwy( p_quat.quat.w * p_quat.quat.y );
			auto const qwz( p_quat.quat.w * p_quat.quat.z );

			p_matrix[0][0] = T( 1 - 2 * ( qyy + qzz ) );
			p_matrix[0][1] = T( 2 * ( qxy + qwz ) );
			p_matrix[0][2] = T( 2 * ( qxz - qwy ) );
			p_matrix[0][3] = T( 0 );

			p_matrix[1][0] = T( 2 * ( qxy - qwz ) );
			p_matrix[1][1] = T( 1 - 2 * ( qxx + qzz ) );
			p_matrix[1][2] = T( 2 * ( qyz + qwx ) );
			p_matrix[1][3] = T( 0 );

			p_matrix[2][0] = T( 2 * ( qxz + qwy ) );
			p_matrix[2][1] = T( 2 * ( qyz - qwx ) );
			p_matrix[2][2] = T( 1 - 2 * ( qxx + qyy ) );
			p_matrix[3][3] = T( 0 );

			p_matrix[3][0] = T( 0 );
			p_matrix[3][1] = T( 0 );
			p_matrix[3][2] = T( 0 );
			p_matrix[3][3] = T( 1 );

			return p_matrix;
		}

		template< typename T, typename U >
		void getrotate( SquareMatrix< T, 4 > const & p_matrix, QuaternionT< U > & p_quat )
		{
			double trace = double( p_matrix[0][0] + p_matrix[1][1] + p_matrix[2][2] );
			double root;

			if ( trace > 0 )
			{
				// |w| > 1/2, may as well choose w > 1/2
				root = std::sqrt( trace + 1 );  // 2w
				p_quat.quat.w = U( 0.5 * root );
				root = 0.5 / root;  // 1/(4w)
				p_quat.quat.x = U( ( p_matrix[2][1] - p_matrix[1][2] ) * root );
				p_quat.quat.y = U( ( p_matrix[0][2] - p_matrix[2][0] ) * root );
				p_quat.quat.z = U( ( p_matrix[1][0] - p_matrix[0][1] ) * root );
			}
			else
			{
				// |w| <= 1/2
				static uint32_t s_next[3] = { 1, 2, 0 };
				uint32_t i = 0;

				if ( p_matrix[1][1] > p_matrix[0][0] )
				{
					i = 1;
				}

				if ( p_matrix[2][2] > p_matrix[i][i] )
				{
					i = 2;
				}

				uint32_t j = s_next[i];
				uint32_t k = s_next[j];
				root = std::sqrt( double( p_matrix[i][i] - p_matrix[j][j] - p_matrix[k][k] + 1 ) );
				U * apkQuat[3] = { &p_quat.quat.x, &p_quat.quat.y, &p_quat.quat.z };
				*apkQuat[i] = U( 0.5 * root );
				root = 0.5 / root;
				*apkQuat[j] = U( double( p_matrix[j][i] + p_matrix[i][j] ) * root );
				*apkQuat[k] = U( double( p_matrix[k][i] + p_matrix[i][k] ) * root );
				p_quat.quat.w = U( double( p_matrix[k][j] - p_matrix[j][k] ) * root );
			}

			point::normalise( p_quat );
		}

		template< typename T >
		SquareMatrix< T, 4 > & yaw( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle )
		{
			return rotate( p_matrix, Quaternion( Point< T, 3 >( 0, 1, 0 ), p_angle ) );
		}

		template< typename T >
		SquareMatrix< T, 4 > & pitch( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle )
		{
			return rotate( p_matrix, Quaternion( Point< T, 3 >( 0, 0, 1 ), p_angle ) );
		}

		template< typename T >
		SquareMatrix< T, 4 > & roll( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle )
		{
			return rotate( p_matrix, Quaternion( Point< T, 3 >( 1, 0, 0 ), p_angle ) );
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & scale( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
		{
			p_matrix[0] *= T( x );
			p_matrix[1] *= T( y );
			p_matrix[2] *= T( z );
			return p_matrix;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & scale( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_scale )
		{
			return scale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & setScale( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
		{
			std::memset( p_matrix.ptr(), 0, sizeof( T ) * 4 * 4 );
			p_matrix[0][0] = T( x );
			p_matrix[1][1] = T( y );
			p_matrix[2][2] = T( z );
			p_matrix[3][3] = T( 1 );
			return p_matrix;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & setScale( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_scale )
		{
			return setScale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & translate( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
		{
			p_matrix[3] = p_matrix[0] * x + p_matrix[1] * y + p_matrix[2] * z + p_matrix[3];
			return p_matrix;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & translate( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_translation )
		{
			return translate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & setTranslate( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
		{
			std::memset( p_matrix.ptr(), 0, sizeof( T ) * 4 * 4 );
			constexpr T const unit( 1 );
			p_matrix[0][0] = unit;
			p_matrix[1][1] = unit;
			p_matrix[2][2] = unit;
			p_matrix[3][0] = T( x );
			p_matrix[3][1] = T( y );
			p_matrix[3][2] = T( z );
			p_matrix[3][3] = unit;
			return p_matrix;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & setTranslate( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_translation )
		{
			return setTranslate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
		}

		template< typename T, typename U, typename V >
		SquareMatrix< T, 4 > & setTransform( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_position, Point< U, 3 > const & p_scale, QuaternionT< V > const & p_orientation )
		{
			// Ordering:
			//    1. Scale
			//    2. Rotate
			//    3. Translate
			setTranslate( p_matrix, p_position );
			rotate( p_matrix, p_orientation );
			return scale( p_matrix, p_scale );
		}

		template< typename T, typename U, typename V >
		SquareMatrix< T, 4 > & transform( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_position, Point< U, 3 > const & p_scale, QuaternionT< V > const & p_orientation )
		{
			SquareMatrix< T, 4 > transform;
			setTransform( transform, p_position, p_scale, p_orientation );
			p_matrix *= transform;
			return p_matrix;
		}

		template< typename T, typename U >
		Point< U, 3 > getTransformed( castor::SquareMatrix< T, 4 > const & p_matrix, castor::Point< U, 3 > const & p_value )
		{
			Point< U, 3 > result;

			result[0] = p_value[0] * p_matrix[0][0] + p_value[1] * p_matrix[1][0] + p_value[2] * p_matrix[2][0] + p_matrix[3][0];
			result[1] = p_value[0] * p_matrix[0][1] + p_value[1] * p_matrix[1][1] + p_value[2] * p_matrix[2][1] + p_matrix[3][1];
			result[2] = p_value[0] * p_matrix[0][2] + p_value[1] * p_matrix[1][2] + p_value[2] * p_matrix[2][2] + p_matrix[3][2];
			U w = p_value[0] * p_matrix[0][3] + p_value[1] * p_matrix[1][3] + p_value[2] * p_matrix[2][3] + p_matrix[3][3];

			result[0] /= w;
			result[1] /= w;
			result[2] /= w;

			return result;
		}

		template< typename T, typename U >
		static Quaternion getTransformed( castor::SquareMatrix< T, 4 > const & p_matrix, castor::QuaternionT< U > const & p_value )
		{
			return p_matrix * p_value;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & perspective( SquareMatrix< T, 4 > & p_matrix, Angle const & p_fovy, U p_aspect, U p_near, U p_far )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml)
			T range = T( ( 1 / tan( p_fovy.radians() * 0.5 ) ) );
			p_matrix.initialise();
			p_matrix[0][0] = T( range / p_aspect );
			p_matrix[1][1] = T( range );
			p_matrix[2][2] = T( ( p_far + p_near ) / ( p_near - p_far ) );
			p_matrix[2][3] = T( -1 );
			p_matrix[3][2] = T( 2 * p_far * p_near / ( p_near - p_far ) );
			return p_matrix;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & perspective( SquareMatrix< T, 4 > & p_matrix, Angle const & p_fovy, U p_aspect, U p_near )
		{
			T range = T( tan( p_fovy.radians() * 0.5 ) * p_near );
			T const left = -range * p_aspect;
			T const right = range * p_aspect;
			T const bottom = -range;
			T const top = range;

			p_matrix.initialise();
			p_matrix[0][0] = ( static_cast< T >( 2 ) * p_near ) / ( right - left );
			p_matrix[1][1] = ( static_cast< T >( 2 ) * p_near ) / ( top - bottom );
			p_matrix[2][2] = -static_cast< T >( 1 );
			p_matrix[2][3] = -static_cast< T >( 1 );
			p_matrix[3][2] = -static_cast< T >( 2 ) * p_near;
			return p_matrix;
		}

		template< typename T >
		SquareMatrix< T, 4 > perspective( Angle const & p_fovy, T p_aspect, T p_near, T p_far )
		{
			SquareMatrix< T, 4 > result;
			perspective( result, p_fovy, p_aspect, p_near, p_far );
			return result;
		}

		template< typename T >
		SquareMatrix< T, 4 > perspective( Angle const & p_fovy, T p_aspect, T p_near )
		{
			SquareMatrix< T, 4 > result;
			perspective( result, p_fovy, p_aspect, p_near );
			return result;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & ortho( SquareMatrix< T, 4 > & p_matrix, U p_left, U p_right, U p_bottom, U p_top, U p_near, U p_far )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml)
			p_matrix.setIdentity();
			p_matrix[0][0] = T( 2 / ( p_right - p_left ) );
			p_matrix[1][1] = T( 2 / ( p_top - p_bottom ) );
			p_matrix[2][2] = T( -2 / ( p_far - p_near ) );
			p_matrix[3][0] = T( -( p_right + p_left ) / ( p_right - p_left ) );
			p_matrix[3][1] = T( -( p_top + p_bottom ) / ( p_top - p_bottom ) );
			p_matrix[3][2] = T( -( p_far + p_near ) / ( p_far - p_near ) );
			return p_matrix;
		}

		template< typename T >
		SquareMatrix< T, 4 > ortho( T p_left, T p_right, T p_bottom, T p_top, T p_near, T p_far )
		{
			SquareMatrix< T, 4 > result;
			ortho( result, p_left, p_right, p_bottom, p_top, p_near, p_far );
			return result;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & frustum( SquareMatrix< T, 4 > & p_matrix, U p_left, U p_right, U p_bottom, U p_top, U p_near, U p_far )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml)
			p_matrix.initialise();
			p_matrix[0][0] = T( ( 2 * p_near ) / ( p_right - p_left ) );
			p_matrix[1][1] = T( ( 2 * p_near ) / ( p_top - p_bottom ) );
			p_matrix[2][0] = T( ( p_right + p_left ) / ( p_right - p_left ) );
			p_matrix[2][1] = T( ( p_top + p_bottom ) / ( p_top - p_bottom ) );
			p_matrix[2][2] = T( -( p_far + p_near ) / ( p_far - p_near ) );
			p_matrix[2][3] = T( -1 );
			p_matrix[3][2] = T( -( 2 * p_far * p_near ) / ( p_far - p_near ) );
			return p_matrix;
		}

		template< typename T >
		SquareMatrix< T, 4 > frustum( T p_left, T p_right, T p_bottom, T p_top, T p_near, T p_far )
		{
			SquareMatrix< T, 4 > result;
			frustum( result, p_left, p_right, p_bottom, p_top, p_near, p_far );
			return result;
		}

		template< typename T, typename U >
		SquareMatrix< T, 4 > & lookAt( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_eye, Point< U, 3 > const & p_center, Point< U, 3 > const & p_up )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml)
			Point< T, 3 > f( point::getNormalised( p_center - p_eye ) );
			Point< T, 3 > s( point::getNormalised( f ^ p_up ) );
			Point< T, 3 > u( s ^ f );
			p_matrix.setIdentity();
			p_matrix[0][0] = s[0];
			p_matrix[0][1] = u[0];
			p_matrix[0][2] = -f[0];
			p_matrix[1][0] = s[1];
			p_matrix[1][1] = u[1];
			p_matrix[1][2] = -f[1];
			p_matrix[2][0] = s[2];
			p_matrix[2][1] = u[2];
			p_matrix[2][2] = -f[2];
			p_matrix[3][0] = -point::dot( s, p_eye );
			p_matrix[3][1] = -point::dot( u, p_eye );
			p_matrix[3][2] =  point::dot( f, p_eye );
			return p_matrix;
		}

		template< typename T >
		SquareMatrix< T, 4 > lookAt( Point< T, 3 > const & p_eye, Point< T, 3 > const & p_center, Point< T, 3 > const & p_up )
		{
			SquareMatrix< T, 4 > result;
			lookAt( result, p_eye, p_center, p_up );
			return result;
		}

		template< typename T >
		SquareMatrix< T, 4 > & switchHand( SquareMatrix< T, 4 > & p_matrix )
		{
			auto col2 = p_matrix[2];
			auto col3 = p_matrix[3];
			std::swap( col2[0], col3[0] );
			std::swap( col2[1], col3[1] );
			std::swap( col2[2], col3[2] );
			std::swap( col2[3], col3[3] );
			return p_matrix;
		}
		template< typename T >
		SquareMatrix< T, 4 > getSwitchHand( SquareMatrix< T, 4 > const & p_matrix )
		{
			SquareMatrix< T, 4 > result( p_matrix );
			switchHand( result );
			return result;
		}
	}
}
