#include "CastorUtils/Math/Quaternion.hpp"

namespace castor
{
	namespace matrix
	{
		template< typename T, typename U >
		Matrix4x4< T > & rotate( Matrix4x4< T > & matrix
			, QuaternionT< U > const & orientation )
		{
			Matrix4x4< T > rotate;
			return matrix *= setRotate( rotate, orientation );
		}

		template< typename T, typename U >
		Matrix4x4< T > & setRotate( Matrix4x4< T > & matrix
			, QuaternionT< U > const & orientation )
		{
			auto const qxx( orientation.quat.x * orientation.quat.x );
			auto const qyy( orientation.quat.y * orientation.quat.y );
			auto const qzz( orientation.quat.z * orientation.quat.z );
			auto const qxz( orientation.quat.x * orientation.quat.z );
			auto const qxy( orientation.quat.x * orientation.quat.y );
			auto const qyz( orientation.quat.y * orientation.quat.z );
			auto const qwx( orientation.quat.w * orientation.quat.x );
			auto const qwy( orientation.quat.w * orientation.quat.y );
			auto const qwz( orientation.quat.w * orientation.quat.z );

			matrix[0][0] = T( 1 - 2 * ( qyy + qzz ) );
			matrix[0][1] = T( 2 * ( qxy + qwz ) );
			matrix[0][2] = T( 2 * ( qxz - qwy ) );
			matrix[0][3] = T( 0 );

			matrix[1][0] = T( 2 * ( qxy - qwz ) );
			matrix[1][1] = T( 1 - 2 * ( qxx + qzz ) );
			matrix[1][2] = T( 2 * ( qyz + qwx ) );
			matrix[1][3] = T( 0 );

			matrix[2][0] = T( 2 * ( qxz + qwy ) );
			matrix[2][1] = T( 2 * ( qyz - qwx ) );
			matrix[2][2] = T( 1 - 2 * ( qxx + qyy ) );
			matrix[3][3] = T( 0 );

			matrix[3][0] = T( 0 );
			matrix[3][1] = T( 0 );
			matrix[3][2] = T( 0 );
			matrix[3][3] = T( 1 );

			return matrix;
		}

		template< typename T, typename U >
		void getRotate( Matrix4x4< T > const & matrix
			, QuaternionT< U > & orientation )
		{
			double trace = double( matrix[0][0] + matrix[1][1] + matrix[2][2] );
			double root;

			if ( trace > 0 )
			{
				// |w| > 1/2, may as well choose w > 1/2
				root = std::sqrt( trace + 1 );  // 2w
				orientation.quat.w = U( 0.5 * root );
				root = 0.5 / root;  // 1/(4w)
				orientation.quat.x = U( ( matrix[2][1] - matrix[1][2] ) * root );
				orientation.quat.y = U( ( matrix[0][2] - matrix[2][0] ) * root );
				orientation.quat.z = U( ( matrix[1][0] - matrix[0][1] ) * root );
			}
			else
			{
				// |w| <= 1/2
				static uint32_t s_next[3] = { 1, 2, 0 };
				uint32_t i = 0;

				if ( matrix[1][1] > matrix[0][0] )
				{
					i = 1;
				}

				if ( matrix[2][2] > matrix[i][i] )
				{
					i = 2;
				}

				uint32_t j = s_next[i];
				uint32_t k = s_next[j];
				root = std::sqrt( double( matrix[i][i] - matrix[j][j] - matrix[k][k] + 1 ) );
				U * apkQuat[3] = { &orientation.quat.x, &orientation.quat.y, &orientation.quat.z };
				*apkQuat[i] = U( 0.5 * root );
				root = 0.5 / root;
				*apkQuat[j] = U( double( matrix[j][i] + matrix[i][j] ) * root );
				*apkQuat[k] = U( double( matrix[k][i] + matrix[i][k] ) * root );
				orientation.quat.w = U( double( matrix[k][j] - matrix[j][k] ) * root );
			}

			point::normalise( orientation );
		}

		template< typename T >
		Matrix4x4< T > & yaw( Matrix4x4< T > & matrix
			, Angle const & angle )
		{
			return rotate( matrix, Quaternion( Point3< T >( 0, 1, 0 ), angle ) );
		}

		template< typename T >
		Matrix4x4< T > & pitch( Matrix4x4< T > & matrix
			, Angle const & angle )
		{
			return rotate( matrix, Quaternion( Point3< T >( 0, 0, 1 ), angle ) );
		}

		template< typename T >
		Matrix4x4< T > & roll( Matrix4x4< T > & matrix
			, Angle const & angle )
		{
			return rotate( matrix, Quaternion( Point3< T >( 1, 0, 0 ), angle ) );
		}

		template< typename T, typename U >
		Matrix4x4< T > & scale( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z )
		{
			matrix[0] *= T( x );
			matrix[1] *= T( y );
			matrix[2] *= T( z );
			return matrix;
		}

		template< typename T, typename U >
		Matrix4x4< T > & scale( Matrix4x4< T > & matrix
			, Point3< U > const & scaling )
		{
			return scale( matrix, scaling[0], scaling[1], scaling[2] );
		}

		template< typename T, typename U >
		Matrix4x4< T > & setScale( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z )
		{
			std::memset( matrix.ptr(), 0, sizeof( T ) * 4 * 4 );
			matrix[0][0] = T( x );
			matrix[1][1] = T( y );
			matrix[2][2] = T( z );
			matrix[3][3] = T( 1 );
			return matrix;
		}

		template< typename T, typename U >
		Matrix4x4< T > & setScale( Matrix4x4< T > & matrix
			, Point3< U > const & scaling )
		{
			return setScale( matrix, scaling[0], scaling[1], scaling[2] );
		}

		template< typename T, typename U >
		Matrix4x4< T > & translate( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z )
		{
			matrix[3] = matrix[0] * x + matrix[1] * y + matrix[2] * z + matrix[3];
			return matrix;
		}

		template< typename T, typename U >
		Matrix4x4< T > & translate( Matrix4x4< T > & matrix
			, Point3< U > const & translation )
		{
			return translate( matrix, translation[0], translation[1], translation[2] );
		}

		template< typename T, typename U >
		Matrix4x4< T > & setTranslate( Matrix4x4< T > & matrix
			, U x
			, U y
			, U z )
		{
			std::memset( matrix.ptr(), 0, sizeof( T ) * 4 * 4 );
			constexpr T const unit( 1 );
			matrix[0][0] = unit;
			matrix[1][1] = unit;
			matrix[2][2] = unit;
			matrix[3][0] = T( x );
			matrix[3][1] = T( y );
			matrix[3][2] = T( z );
			matrix[3][3] = unit;
			return matrix;
		}

		template< typename T, typename U >
		Matrix4x4< T > & setTranslate( Matrix4x4< T > & matrix
			, Point3< U > const & translation )
		{
			return setTranslate( matrix, translation[0], translation[1], translation[2] );
		}

		template< typename T, typename U, typename V >
		Matrix4x4< T > & setTransform( Matrix4x4< T > & matrix
			, Point3< U > const & position
			, Point3< U > const & scaling
			, QuaternionT< V > const & orientation )
		{
			// Ordering:
			//    1. Scale
			//    2. Rotate
			//    3. Translate
			setTranslate( matrix, position );
			rotate( matrix, orientation );
			return scale( matrix, scaling );
		}

		template< typename T, typename U, typename V >
		Matrix4x4< T > & transform( Matrix4x4< T > & matrix
			, Point3< U > const & position
			, Point3< U > const & scaling
			, QuaternionT< V > const & orientation )
		{
			Matrix4x4< T > transform;
			setTransform( transform, position, scaling, orientation );
			matrix *= transform;
			return matrix;
		}

		template< typename T, typename U >
		Point3< U > getTransformed( castor::Matrix4x4< T > const & matrix
			, castor::Point3< U > const & value )
		{
			Point3< U > result;

			result[0] = value[0] * matrix[0][0] + value[1] * matrix[1][0] + value[2] * matrix[2][0] + matrix[3][0];
			result[1] = value[0] * matrix[0][1] + value[1] * matrix[1][1] + value[2] * matrix[2][1] + matrix[3][1];
			result[2] = value[0] * matrix[0][2] + value[1] * matrix[1][2] + value[2] * matrix[2][2] + matrix[3][2];
			U w = value[0] * matrix[0][3] + value[1] * matrix[1][3] + value[2] * matrix[2][3] + matrix[3][3];

			result[0] /= w;
			result[1] /= w;
			result[2] /= w;

			return result;
		}

		template< typename T, typename U >
		static Quaternion getTransformed( castor::Matrix4x4< T > const & matrix
			, castor::QuaternionT< U > const & value )
		{
			return matrix * value;
		}

		template< typename T, typename U >
		Matrix4x4< T > & perspective( Matrix4x4< T > & matrix
			, Angle const & fovy
			, U aspect
			, U znear
			, U zfar )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml)
			T range = T( tan( fovy.radians() * 0.5 ) );
			matrix.initialise();
			matrix[0][0] = T( 1 / ( range * aspect ) );
			matrix[1][1] = T( 1 / range );
			matrix[2][2] = T( -( zfar + znear ) / ( zfar - znear ) );
			matrix[2][3] = T( -1 );
			matrix[3][2] = T( -2 * zfar * znear / ( zfar - znear ) );
			return matrix;
		}

		template< typename T, typename U >
		Matrix4x4< T > & perspective( Matrix4x4< T > & matrix
			, Angle const & fovy
			, U aspect
			, U znear )
		{
			T range = T( tan( fovy.radians() * 0.5 ) * znear );
			T const left = -range * aspect;
			T const right = range * aspect;
			T const bottom = -range;
			T const top = range;

			matrix.initialise();
			matrix[0][0] = ( static_cast< T >( 2 ) * znear ) / ( right - left );
			matrix[1][1] = ( static_cast< T >( 2 ) * znear ) / ( top - bottom );
			matrix[2][2] = -static_cast< T >( 1 );
			matrix[2][3] = -static_cast< T >( 1 );
			matrix[3][2] = -static_cast< T >( 2 ) * znear;
			return matrix;
		}

		template< typename T >
		Matrix4x4< T > perspective( Angle const & fovy
			, T aspect
			, T znear
			, T zfar )
		{
			Matrix4x4< T > result;
			perspective( result, fovy, aspect, znear, zfar );
			return result;
		}

		template< typename T >
		Matrix4x4< T > perspective( Angle const & fovy
			, T aspect
			, T znear )
		{
			Matrix4x4< T > result;
			perspective( result, fovy, aspect, znear );
			return result;
		}

		template< typename T, typename U >
		Matrix4x4< T > & ortho( Matrix4x4< T > & matrix
			, U left
			, U right
			, U bottom
			, U top
			, U znear
			, U zfar )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml)
			matrix.setIdentity();
			matrix[0][0] = T( 2 / ( right - left ) );
			matrix[1][1] = T( 2 / ( top - bottom ) );
			matrix[2][2] = T( -2 / ( zfar - znear ) );
			matrix[3][0] = T( -( right + left ) / ( right - left ) );
			matrix[3][1] = T( -( top + bottom ) / ( top - bottom ) );
			matrix[3][2] = T( -( zfar + znear ) / ( zfar - znear ) );
			return matrix;
		}

		template< typename T >
		Matrix4x4< T > ortho( T left
			, T right
			, T bottom
			, T top
			, T znear
			, T zfar )
		{
			Matrix4x4< T > result;
			ortho( result, left, right, bottom, top, znear, zfar );
			return result;
		}

		template< typename T, typename U >
		Matrix4x4< T > & frustum( Matrix4x4< T > & matrix
			, U left
			, U right
			, U bottom
			, U top
			, U znear
			, U zfar )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml)
			matrix.initialise();
			matrix[0][0] = T( ( 2 * znear ) / ( right - left ) );
			matrix[1][1] = T( ( 2 * znear ) / ( top - bottom ) );
			matrix[2][0] = T( ( right + left ) / ( right - left ) );
			matrix[2][1] = T( ( top + bottom ) / ( top - bottom ) );
			matrix[2][2] = T( -( zfar + znear ) / ( zfar - znear ) );
			matrix[2][3] = T( -1 );
			matrix[3][2] = T( -( 2 * zfar * znear ) / ( zfar - znear ) );
			return matrix;
		}

		template< typename T >
		Matrix4x4< T > frustum( T left
			, T right
			, T bottom
			, T top
			, T znear
			, T zfar )
		{
			Matrix4x4< T > result;
			frustum( result, left, right, bottom, top, znear, zfar );
			return result;
		}

		template< typename T, typename U >
		Matrix4x4< T > & lookAt( Matrix4x4< T > & matrix
			, Point3< U > const & eye
			, Point3< U > const & center
			, Point3< U > const & up )
		{
			// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml)
			Point3< T > f( point::getNormalised( center - eye ) );
			Point3< T > s( point::getNormalised( castor::point::cross( f, up ) ) );
			Point3< T > u( castor::point::cross( s, f ) );
			matrix.setIdentity();
			matrix[0][0] = s[0];
			matrix[0][1] = u[0];
			matrix[0][2] = -f[0];
			matrix[1][0] = s[1];
			matrix[1][1] = u[1];
			matrix[1][2] = -f[1];
			matrix[2][0] = s[2];
			matrix[2][1] = u[2];
			matrix[2][2] = -f[2];
			matrix[3][0] = -point::dot( s, eye );
			matrix[3][1] = -point::dot( u, eye );
			matrix[3][2] =  point::dot( f, eye );
			return matrix;
		}

		template< typename T >
		Matrix4x4< T > lookAt( Point3< T > const & eye
			, Point3< T > const & center
			, Point3< T > const & up )
		{
			Matrix4x4< T > result;
			lookAt( result, eye, center, up );
			return result;
		}

		template< typename T >
		Matrix4x4< T > & switchHand( Matrix4x4< T > & matrix )
		{
			auto col2 = matrix[2];
			auto col3 = matrix[3];
			std::swap( col2[0], col3[0] );
			std::swap( col2[1], col3[1] );
			std::swap( col2[2], col3[2] );
			std::swap( col2[3], col3[3] );
			return matrix;
		}
		template< typename T >
		Matrix4x4< T > getSwitchHand( Matrix4x4< T > const & matrix )
		{
			Matrix4x4< T > result( matrix );
			switchHand( result );
			return result;
		}
	}
}
