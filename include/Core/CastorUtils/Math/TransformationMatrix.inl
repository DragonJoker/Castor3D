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
			float t = matrix[0][0] + matrix[1][1] + matrix[2][2];

			// large enough
			if ( t > 0.0f )
			{
				float s = std::sqrt( 1 + t ) * 2.0f;
				orientation.quat.x = ( matrix[1][2] - matrix[2][1] ) / s;
				orientation.quat.y = ( matrix[2][0] - matrix[0][2] ) / s;
				orientation.quat.z = ( matrix[0][1] - matrix[1][0] ) / s;
				orientation.quat.w = 0.25f * s;
			} // else we have to check several cases
			else if ( matrix[0][0] > matrix[1][1] && matrix[0][0] > matrix[2][2] )
			{
				// Column 0:
				float s = std::sqrt( 1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2] ) * 2.0f;
				orientation.quat.x = 0.25f * s;
				orientation.quat.y = ( matrix[0][1] + matrix[1][0] ) / s;
				orientation.quat.z = ( matrix[2][0] + matrix[0][2] ) / s;
				orientation.quat.w = ( matrix[1][2] - matrix[2][1] ) / s;
			}
			else if ( matrix[1][1] > matrix[2][2] )
			{
				// Column 1:
				float s = std::sqrt( 1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2] ) * 2.0f;
				orientation.quat.x = ( matrix[0][1] + matrix[1][0] ) / s;
				orientation.quat.y = 0.25f * s;
				orientation.quat.z = ( matrix[1][2] + matrix[2][1] ) / s;
				orientation.quat.w = ( matrix[2][0] - matrix[0][2] ) / s;
			}
			else
			{
				// Column 2:
				float s = std::sqrt( 1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1] ) * 2.0f;
				orientation.quat.x = ( matrix[2][0] + matrix[0][2] ) / s;
				orientation.quat.y = ( matrix[1][2] + matrix[2][1] ) / s;
				orientation.quat.z = 0.25f * s;
				orientation.quat.w = ( matrix[0][1] - matrix[1][0] ) / s;
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

		template< typename T, typename U, typename V >
		static void decompose( Matrix4x4< T > const & matrix
			, Point3< U > & position
			, Point3< U > & scaling
			, QuaternionT< V > & rotation )
		{
			/* extract translation */
			position->x = U( matrix[3][0] );
			position->y = U( matrix[3][1] );
			position->z = U( matrix[3][2] );

			/* extract the columns of the matrix. */
			Point3< U > cols[3] = { Point3< U >{ matrix[0] }
				, Point3< U >{ matrix[1] }
				, Point3< U >{ matrix[2] } };

			/* extract the scaling factors */
			scaling->x = U( point::length( cols[0] ) );
			scaling->y = U( point::length( cols[1] ) );
			scaling->z = U( point::length( cols[2] ) );

			/* and the sign of the scaling */
			if ( matrix.getDeterminant() < 0 )
			{
				scaling = -scaling;
			}

			/* and remove all scaling from the matrix */
			if ( scaling->x )
			{
				cols[0] /= scaling->x;
			}

			if ( scaling->y )
			{
				cols[1] /= scaling->y;
			}

			if ( scaling->z )
			{
				cols[2] /= scaling->z;
			}

			// build a 3x3 rotation matrix
			Matrix4x4< U > m{ 1.0f };
			m[0][0] = cols[0][0];
			m[0][1] = cols[0][1];
			m[0][2] = cols[0][2];
			m[1][0] = cols[1][0];
			m[1][1] = cols[1][1];
			m[1][2] = cols[1][2];
			m[2][0] = cols[2][0];
			m[2][1] = cols[2][1];
			m[2][2] = cols[2][2];

			// and generate the rotation quaternion from it
			getRotate( m, rotation );
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
			matrix[0][0] = s->x;
			matrix[1][0] = s->y;
			matrix[2][0] = s->z;
			matrix[0][1] = u->x;
			matrix[1][1] = u->y;
			matrix[2][1] = u->z;
			matrix[0][2] = -f->x;
			matrix[1][2] = -f->y;
			matrix[2][2] = -f->z;
			matrix[3][0] = -point::dot( s, eye );
			matrix[3][1] = -point::dot( u, eye );
			matrix[3][2] = point::dot( f, eye );
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

		template< typename T, typename U >
		Matrix3x3< T > & rotate( Matrix3x3< T > & matrix
			, Angle orientation )
		{
			Matrix3x3< T > rotate;
			return matrix *= setRotate( rotate, orientation );
		}

		template< typename T, typename U >
		Matrix3x3< T > & setRotate( Matrix3x3< T > & matrix
			, Angle orientation )
		{
			auto cosT = orientation.cos();
			auto sinT = orientation.sin();

			matrix[0][0] = T( cosT );
			matrix[0][1] = T( sinT );
			matrix[0][2] = T( 0 );

			matrix[1][0] = T( -sinT );
			matrix[1][1] = T( cosT );
			matrix[1][2] = T( 0 );

			matrix[2][0] = T( 0 );
			matrix[2][1] = T( 0 );
			matrix[2][2] = T( 1 );

			return matrix;
		}

		template< typename T, typename U >
		void getRotate( Matrix3x3< T > const & matrix
			, Angle & orientation )
		{
		}

		template< typename T, typename U >
		Matrix3x3< T > & scale( Matrix3x3< T > & matrix
			, U x
			, U y )
		{
			matrix[0] *= T( x );
			matrix[1] *= T( y );
			return matrix;
		}

		template< typename T, typename U >
		Matrix3x3< T > & scale( Matrix3x3< T > & matrix
			, Point2< U > const & scaling )
		{
			return scale( matrix, scaling[0], scaling[1] );
		}

		template< typename T, typename U >
		Matrix3x3< T > & setScale( Matrix3x3< T > & matrix
			, U x
			, U y )
		{
			std::memset( matrix.ptr(), 0, sizeof( T ) * 3 * 3 );
			matrix[0][0] = T( x );
			matrix[1][1] = T( y );
			matrix[2][2] = T( 1 );
			return matrix;
		}

		template< typename T, typename U >
		Matrix3x3< T > & setScale( Matrix3x3< T > & matrix
			, Point2< U > const & scaling )
		{
			return setScale( matrix, scaling[0], scaling[1] );
		}

		template< typename T, typename U >
		Matrix3x3< T > & translate( Matrix3x3< T > & matrix
			, U x
			, U y )
		{
			matrix[2] = matrix[0] * x + matrix[1] * y + matrix[2];
			return matrix;
		}

		template< typename T, typename U >
		Matrix3x3< T > & translate( Matrix3x3< T > & matrix
			, Point2< U > const & translation )
		{
			return translate( matrix, translation[0], translation[1] );
		}

		template< typename T, typename U >
		Matrix3x3< T > & setTranslate( Matrix3x3< T > & matrix
			, U x
			, U y )
		{
			std::memset( matrix.ptr(), 0, sizeof( T ) * 3 * 3 );
			constexpr T const unit( 1 );
			matrix[0][0] = unit;
			matrix[1][1] = unit;
			matrix[2][0] = T( x );
			matrix[2][1] = T( y );
			matrix[2][2] = unit;
			return matrix;
		}

		template< typename T, typename U >
		Matrix3x3< T > & setTranslate( Matrix3x3< T > & matrix
			, Point2< U > const & translation )
		{
			return setTranslate( matrix, translation[0], translation[1] );
		}

		template< typename T, typename U, typename V >
		Matrix3x3< T > & setTransform( Matrix3x3< T > & matrix
			, Point2< U > const & position
			, Point2< U > const & scaling
			, Angle orientation )
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
		Matrix3x3< T > & transform( Matrix3x3< T > & matrix
			, Point2< U > const & position
			, Point2< U > const & scaling
			, Angle orientation )
		{
			Matrix3x3< T > transform;
			setTransform( transform, position, scaling, orientation );
			matrix *= transform;
			return matrix;
		}
	}
}
