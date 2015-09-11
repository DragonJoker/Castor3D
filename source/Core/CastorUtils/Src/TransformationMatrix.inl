namespace Castor
{
	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::rotate( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle, Point< U, 3 > const & p_axis )
	{
		SquareMatrix< T, 4 > l_rotate;
		return p_matrix *= matrix::set_rotate( l_rotate, p_angle, p_axis );
	}

	template< typename T >
	SquareMatrix< T, 4 > & matrix::rotate( SquareMatrix< T, 4 > & p_matrix, Quaternion const & p_quat )
	{
		Point3r l_axis;
		Angle l_angle;
		p_quat.ToAxisAngle( l_axis, l_angle );
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::set_rotate( SquareMatrix< T, 4 > & p_matrix, Angle const & p_angle, Point< U, 3 > const & p_axis )
	{
		T l_cos = T( p_angle.Cos() );
		T l_sin = T( p_angle.Sin() );
		Point< U, 3 > l_axis( point::get_normalised( p_axis ) );
		Point< T, 3 > l_temp( ( T( 1 ) - l_cos ) * l_axis );
		p_matrix[0][0] = l_cos + l_temp[0] * l_axis[0];
		p_matrix[0][1] =     0 + l_temp[0] * l_axis[1] + l_sin * l_axis[2];
		p_matrix[0][2] =     0 + l_temp[0] * l_axis[2] - l_sin * l_axis[1];

		p_matrix[1][0] =     0 + l_temp[1] * l_axis[0] - l_sin * l_axis[2];
		p_matrix[1][1] = l_cos + l_temp[1] * l_axis[1];
		p_matrix[1][2] =     0 + l_temp[1] * l_axis[2] + l_sin * l_axis[0];

		p_matrix[2][0] =     0 + l_temp[2] * l_axis[0] + l_sin * l_axis[1];
		p_matrix[2][1] =     0 + l_temp[2] * l_axis[1] - l_sin * l_axis[0];
		p_matrix[2][2] = l_cos + l_temp[2] * l_axis[2];
		p_matrix[3][3] = 1;
		return p_matrix;
	}

	template< typename T >
	SquareMatrix< T, 4 > & matrix::set_rotate( SquareMatrix< T, 4 > & p_matrix, Quaternion const & p_quat )
	{
		Point3r l_axis;
		Angle l_angle;
		p_quat.ToAxisAngle( l_axis, l_angle );
		return matrix::set_rotate( p_matrix, l_angle, l_axis );
		p_quat.ToRotationMatrix( p_matrix );
		return p_matrix;
	}

	template< typename T >
	void matrix::get_rotate( SquareMatrix< T, 4 > const & p_matrix, Quaternion & p_quat )
	{
		p_quat.FromRotationMatrix( p_matrix );
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
		p_matrix = l_translate * l_rotate * l_scale;
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
		p_matrix *= l_translate * l_rotate * l_scale;
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::perspective( SquareMatrix< T, 4 > & p_matrix, Angle const & p_aFOVY, U aspect, U zNear, U zFar )
	{
		T range = T( ( 1 / tan( p_aFOVY.Radians() / 2 ) ) );
		p_matrix.initialise();
		p_matrix[0][0] = T( range / aspect );
		p_matrix[1][1] = T( range );
		p_matrix[2][2] = T( - ( zFar + zNear ) / ( zFar - zNear ) );
		p_matrix[2][3] = T( - 1 );
		p_matrix[3][2] = T( - 2 * zFar * zNear / ( zFar - zNear ) );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::ortho( SquareMatrix< T, 4 > & p_matrix, U left, U right, U bottom, U top )
	{
		p_matrix.set_identity();
		p_matrix[0][0] = T( 2 / ( right - left ) );
		p_matrix[1][1] = T( 2 / ( top - bottom ) );
		p_matrix[2][2] = - T( 1 );
		p_matrix[3][0] = T( -( right + left ) / ( right - left ) );
		p_matrix[3][1] = T( -( top + bottom ) / ( top - bottom ) );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::ortho( SquareMatrix< T, 4 > & p_matrix, U left, U right, U bottom, U top, U zNear, U zFar )
	{
		p_matrix.set_identity();
		p_matrix[0][0] = T( 2 / ( right - left ) );
		p_matrix[1][1] = T( 2 / ( top - bottom ) );
		p_matrix[2][2] = T( -2 / ( zFar - zNear ) );
		p_matrix[3][0] = T( -( right + left ) / ( right - left ) );
		p_matrix[3][1] = T( -( top + bottom ) / ( top - bottom ) );
		p_matrix[3][2] = T( -( zFar + zNear ) / ( zFar - zNear ) );
		p_matrix[3][3] = 1;
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::frustum( SquareMatrix< T, 4 > & p_matrix, U left, U right, U bottom, U top, U nearVal, U farVal )
	{
		p_matrix.initialise();
		p_matrix[0][0] = T( ( 2 * nearVal ) / ( right - left ) );
		p_matrix[1][1] = T( ( 2 * nearVal ) / ( top - bottom ) );
		p_matrix[2][0] = T( ( right + left ) / ( right - left ) );
		p_matrix[2][1] = T( ( top + bottom ) / ( top - bottom ) );
		p_matrix[2][2] = T( ( nearVal + farVal ) / ( nearVal - farVal ) );
		p_matrix[2][3] = T( -1 );
		p_matrix[3][2] = T( ( 2 * farVal * nearVal ) / ( nearVal - farVal ) );
		return p_matrix;
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > & matrix::look_at( SquareMatrix< T, 4 > & p_matrix, Point< U, 3 > const & p_ptEye, Point< U, 3 > const & p_ptCenter, Point< U, 3 > const & p_ptUp )
	{
		Point< T, 3 > l_f( point::get_normalised( p_ptCenter - p_ptEye ) );
		Point< T, 3 > l_s( point::get_normalised( l_f ^ p_ptUp ) );
		Point< T, 3 > l_u( point::get_normalised( l_s ^ l_f ) );
		p_matrix.set_identity();
		p_matrix[0][0] =  l_s[0];
		p_matrix[1][0] =  l_s[1];
		p_matrix[2][0] =  l_s[2];
		p_matrix[0][1] =  l_u[0];
		p_matrix[1][1] =  l_u[1];
		p_matrix[2][1] =  l_u[2];
		p_matrix[0][2] = -l_f[0];
		p_matrix[1][2] = -l_f[1];
		p_matrix[2][2] = -l_f[2];
		p_matrix[3][0] = -point::dot( l_s, p_ptEye );
		p_matrix[3][1] = -point::dot( l_u, p_ptEye );
		p_matrix[3][2] =  point::dot( l_f, p_ptEye );
		return p_matrix;
	}

	template <typename T, typename U, uint32_t Count>
	Point<U, Count> matrix::mult( SquareMatrix< T, 4 > const & p_matrix, Point<U, Count> const & p_vertex )
	{
		return MtxMultiplicator< T, U, Count >()( p_matrix, p_vertex );
	}

	template< typename T, typename U >
	SquareMatrix< T, 4 > matrix::mult( SquareMatrix< T, 4 > const & p_matrixA, SquareMatrix<U, 4> const & p_matrixB )
	{
		return p_matrixA.multiply( p_matrixB );
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
		SquareMatrix< T, 4 > l_mtxReturn( p_matrix );
		switch_hand( l_mtxReturn );
		return l_mtxReturn;
	}
}
