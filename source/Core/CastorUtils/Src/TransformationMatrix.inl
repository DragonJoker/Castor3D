#include "Quaternion.hpp"

namespace Castor
{
	template< typename T >
	SquareMatrix< T, 4 > & matrix::rotate( SquareMatrix< T, 4 > & p_matrix, Quaternion const & p_quat )
	{
		SquareMatrix< T, 4 > l_rotate;
		return p_matrix *= matrix::set_rotate( l_rotate, p_quat );
	}

	template< typename T >
	SquareMatrix< T, 4 > & matrix::set_rotate( SquareMatrix< T, 4 > & p_matrix, Quaternion const & p_quat )
	{
		p_matrix[0][0] = T( 1.0 - 2.0f * p_quat.y * p_quat.y - 2.0 * p_quat.z * p_quat.z );
		p_matrix[0][1] = T( 2.0 * p_quat.x * p_quat.y - 2.0 * p_quat.z * p_quat.w );
		p_matrix[0][2] = T( 2.0 * p_quat.x * p_quat.z + 2.0 * p_quat.y * p_quat.w );
		p_matrix[0][3] = T( 0.0 );

		p_matrix[1][0] = T( 2.0 * p_quat.x * p_quat.y + 2.0 * p_quat.z * p_quat.w );
		p_matrix[1][1] = T( 1.0 - 2.0f * p_quat.x * p_quat.x - 2.0 * p_quat.z * p_quat.z );
		p_matrix[1][2] = T( 2.0 * p_quat.y * p_quat.z - 2.0 * p_quat.x * p_quat.w );
		p_matrix[1][3] = T( 0.0 );

		p_matrix[2][0] = T( 2.0 * p_quat.x * p_quat.z - 2.0 * p_quat.y * p_quat.w );
		p_matrix[2][1] = T( 2.0 * p_quat.y * p_quat.z + 2.0 * p_quat.x * p_quat.w );
		p_matrix[2][2] = T( 1.0 - 2.0 * p_quat.x * p_quat.x - 2.0 * p_quat.y * p_quat.y );
		p_matrix[3][3] = T( 0.0 );

		p_matrix[3][0] = T( 0.0 );
		p_matrix[3][1] = T( 0.0 );
		p_matrix[3][2] = T( 0.0 );
		p_matrix[3][3] = T( 1.0 );

		return p_matrix;
	}

	template< typename T >
	void matrix::get_rotate( SquareMatrix< T, 4 > const & p_matrix, Quaternion & p_quat )
	{
		double l_trace = double( p_matrix[0][0] + p_matrix[1][1] + p_matrix[2][2] );
		double l_root;

		if ( l_trace > 0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			l_root = std::sqrt( l_trace + 1 );  // 2w
			p_quat.w = T( 0.5 * l_root );
			l_root = 0.5 / l_root;  // 1/(4w)
			p_quat.x = ( p_matrix[2][1] - p_matrix[1][2] ) * l_root;
			p_quat.y = ( p_matrix[0][2] - p_matrix[2][0] ) * l_root;
			p_quat.z = ( p_matrix[1][0] - p_matrix[0][1] ) * l_root;
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
			l_root = std::sqrt( double( p_matrix[i][i] - p_matrix[j][j] - p_matrix[k][k] + 1 ) );
			double * l_apkQuat[3] = { &p_quat.x, &p_quat.y, &p_quat.z };
			*l_apkQuat[i] = 0.5 * l_root;
			l_root = 0.5 / l_root;
			*l_apkQuat[j] = double( p_matrix[j][i] + p_matrix[i][j] ) * l_root;
			*l_apkQuat[k] = double( p_matrix[k][i] + p_matrix[i][k] ) * l_root;
			p_quat.w = double( p_matrix[k][j] - p_matrix[j][k] ) * l_root;
		}

		point::normalise( p_quat );
	}

	template< typename T >
	SquareMatrix< T, 4 > & matrix::yaw( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle )
	{
		return matrix::rotate( p_matrix, p_angle, Point< T, 3 >( 0, 1, 0 ) );
	}

	template< typename T >
	SquareMatrix< T, 4 > & matrix::pitch( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle )
	{
		return matrix::rotate( p_matrix, p_angle, Point< T, 3 >( 0, 0, 1 ) );
	}

	template< typename T >
	SquareMatrix< T, 4 > & matrix::roll( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle )
	{
		return matrix::rotate( p_matrix, p_angle, Point< T, 3 >( 1, 0, 0 ) );
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::scale( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
	{
		p_matrix[0][0] = T( p_matrix[0][0] * x );
		p_matrix[1][1] = T( p_matrix[1][1] * y );
		p_matrix[2][2] = T( p_matrix[2][2] * z );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::scale( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_scale )
	{
		return matrix::scale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::set_scale( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
	{
		std::memset( p_matrix.ptr(), 0, sizeof( T ) * 4 * 4 );
		p_matrix[0][0] = T( x );
		p_matrix[1][1] = T( y );
		p_matrix[2][2] = T( z );
		p_matrix[3][3] = T( 1 );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::set_scale( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_scale )
	{
		return matrix::set_scale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
	}

	template< typename T, typename U >
	void matrix::get_scale( SquareMatrix< T, 4 > const & p_matrix, Point< U, 3 > & p_scale )
	{
		p_scale[0] = U( p_matrix[0][0] );
		p_scale[1] = U( p_matrix[1][1] );
		p_scale[2] = U( p_matrix[2][2] );
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::translate( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
	{
		p_matrix[3][0] += T( x );
		p_matrix[3][1] += T( y );
		p_matrix[3][2] += T( z );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::translate( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_translation )
	{
		return matrix::translate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::set_translate( SquareMatrix< T, 4 > & p_matrix, U x, U y, U z )
	{
		std::memset( p_matrix.ptr(), 0, sizeof( T ) * 4 * 4 );
		T l_unit( 1 );
		p_matrix[0][0] = l_unit;
		p_matrix[1][1] = l_unit;
		p_matrix[2][2] = l_unit;
		p_matrix[3][0] = T( x );
		p_matrix[3][1] = T( y );
		p_matrix[3][2] = T( z );
		p_matrix[3][3] = l_unit;
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::set_translate( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_translation )
	{
		return set_translate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
	}

	template< typename T, typename U >
	void matrix::get_translate( SquareMatrix< T, 4 > const & p_matrix, Point< U, 3 > & p_translation )
	{
		p_translation[0] = Policy< U >::convert( p_matrix[3][0] );
		p_translation[1] = Policy< U >::convert( p_matrix[3][1] );
		p_translation[2] = Policy< U >::convert( p_matrix[3][2] );
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::set_transform( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_ptPosition, Point< U, 3 > const & p_ptScale, Quaternion const & p_qOrientation )
	{
		SquareMatrix< T, 4 > l_scale;
		SquareMatrix< T, 4 > l_rotate;
		SquareMatrix< T, 4 > l_translate;
		matrix::set_scale( l_scale, p_ptScale );
		matrix::set_rotate( l_rotate, p_qOrientation );
		matrix::set_translate( l_translate, p_ptPosition );
		p_matrix = l_scale * l_rotate * l_translate;
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::transform( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_ptPosition, Point< U, 3 > const & p_ptScale, Quaternion const & p_qOrientation )
	{
		SquareMatrix< T, 4 > l_scale;
		SquareMatrix< T, 4 > l_rotate;
		SquareMatrix< T, 4 > l_translate;
		matrix::set_scale( l_scale, p_ptScale );
		matrix::set_rotate( l_rotate, p_qOrientation );
		matrix::set_translate( l_translate, p_ptPosition );
		p_matrix *= l_rotate * l_translate * l_scale;
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::perspective( SquareMatrix< T, 4 > & p_matrix, Angle const & p_aFOVY, U p_aspect, U p_near, U p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml)
		T l_range = T( ( 1 / tan( p_aFOVY.radians() * 0.5 ) ) );
		p_matrix.initialise();
		p_matrix[0][0] = T( l_range / p_aspect );
		p_matrix[1][1] = T( l_range );
		p_matrix[2][2] = T( ( p_far + p_near ) / ( p_near - p_far ) );
		p_matrix[2][3] = T( -1 );
		p_matrix[3][2] = T( 2 * p_far * p_near / ( p_near - p_far ) );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::ortho( SquareMatrix< T, 4 > & p_matrix, U p_left, U p_right, U p_bottom, U p_top, U p_near, U p_far )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml)
		p_matrix.set_identity();
		p_matrix[0][0] = T( 2 / ( p_right - p_left ) );
		p_matrix[1][1] = T( 2 / ( p_top - p_bottom ) );
		p_matrix[2][2] = T( -2 / ( p_far - p_near ) );
		p_matrix[3][0] = T( -( p_right + p_left ) / ( p_right - p_left ) );
		p_matrix[3][1] = T( -( p_top + p_bottom ) / ( p_top - p_bottom ) );
		p_matrix[3][2] = T( -( p_far + p_near ) / ( p_far - p_near ) );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::frustum( SquareMatrix< T, 4 > & p_matrix, U p_left, U p_right, U p_bottom, U p_top, U p_near, U p_far )
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

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::look_at( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_eye, Point< U, 3 > const & p_center, Point< U, 3 > const & p_up )
	{
		// OpenGL right handed (cf. https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml)
		Point< T, 3 > l_f( point::get_normalised( p_center - p_eye ) );
		Point< T, 3 > l_u( point::get_normalised( p_up ) );
		Point< T, 3 > l_s( point::get_normalised( l_f ^ l_u ) );
		l_u = l_s ^ l_f;
		p_matrix.set_identity();
		p_matrix[0][0] =  l_s[0];
		p_matrix[0][1] =  l_u[0];
		p_matrix[0][2] = -l_f[0];
		p_matrix[1][0] =  l_s[1];
		p_matrix[1][1] =  l_u[1];
		p_matrix[1][2] = -l_f[1];
		p_matrix[2][0] =  l_s[2];
		p_matrix[2][1] =  l_u[2];
		p_matrix[2][2] = -l_f[2];
		p_matrix[3][0] = 0;
		p_matrix[3][1] = 0;
		p_matrix[3][2] = 0;
		return p_matrix;
	}

	template< typename T >
	SquareMatrix< T, 4 > & matrix::switch_hand( SquareMatrix< T, 4 > & p_matrix )
	{
		auto l_col2 = p_matrix[2];
		auto l_col3 = p_matrix[3];
		std::swap( l_col2[0], l_col3[0] );
		std::swap( l_col2[1], l_col3[1] );
		std::swap( l_col2[2], l_col3[2] );
		std::swap( l_col2[3], l_col3[3] );
		return p_matrix;
	}
	template< typename T >
	SquareMatrix< T, 4 > matrix::get_switch_hand( SquareMatrix< T, 4 > const & p_matrix )
	{
		SquareMatrix< T, 4 > l_return( p_matrix );
		switch_hand( l_return );
		return l_return;
	}
}
