namespace Castor
{
	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::rotate( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle, Point< TypeB, 3 > const & p_axis )
	{
		return MtxUtils::rotate( p_matrix, Quaternion( p_axis, p_angle ) );
	}

	template< typename TypeA >
	SquareMatrix< TypeA, 4 > & MtxUtils::rotate( SquareMatrix< TypeA, 4 > & p_matrix, Quaternion const & p_quat )
	{
		SquareMatrix< TypeA, 4 > l_matrix;
		p_quat.ToRotationMatrix( l_matrix );
		p_matrix *= l_matrix;
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_rotate( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle, Point< TypeB, 3 > const & p_axis )
	{
		return MtxUtils::set_rotate( p_matrix, Quaternion( p_axis, p_angle ) );
	}

	template< typename TypeA >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_rotate( SquareMatrix< TypeA, 4 > & p_matrix, Quaternion const & p_quat )
	{
		p_quat.ToRotationMatrix( p_matrix );
		return p_matrix;
	}

	template< typename TypeA >
	void MtxUtils::get_rotate( SquareMatrix< TypeA, 4 > const & p_matrix, Quaternion & p_quat )
	{
		p_quat.FromRotationMatrix( p_matrix );
	}

	template< typename TypeA >
	SquareMatrix< TypeA, 4 > & MtxUtils::yaw( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle )
	{
		return MtxUtils::rotate( p_matrix, p_angle, Point< TypeA, 3 >( 0, 1, 0 ) );
	}

	template< typename TypeA >
	SquareMatrix< TypeA, 4 > & MtxUtils::pitch( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle )
	{
		return MtxUtils::rotate( p_matrix, p_angle, Point< TypeA, 3 >( 0, 0, 1 ) );
	}

	template< typename TypeA >
	SquareMatrix< TypeA, 4 > & MtxUtils::roll( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_angle )
	{
		return MtxUtils::rotate( p_matrix, p_angle, Point< TypeA, 3 >( 1, 0, 0 ) );
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::scale( SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
	{
		p_matrix[0][0] = TypeA( p_matrix[0][0] * x );
		p_matrix[1][1] = TypeA( p_matrix[1][1] * y );
		p_matrix[2][2] = TypeA( p_matrix[2][2] * z );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::scale( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_scale )
	{
		return MtxUtils::scale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_scale( SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
	{
		std::memset( p_matrix.ptr(), 0, sizeof( TypeA ) * 4 * 4 );
		p_matrix[0][0] = TypeA( x );
		p_matrix[1][1] = TypeA( y );
		p_matrix[2][2] = TypeA( z );
		p_matrix[3][3] = TypeA( 1 );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_scale( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_scale )
	{
		return MtxUtils::set_scale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
	}

	template< typename TypeA, typename TypeB >
	void MtxUtils::get_scale( SquareMatrix< TypeA, 4 > const & p_matrix, Point< TypeB, 3 > & p_scale )
	{
		p_scale[0] = TypeB( p_matrix[0][0] );
		p_scale[1] = TypeB( p_matrix[1][1] );
		p_scale[2] = TypeB( p_matrix[2][2] );
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::translate( SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
	{
		p_matrix[3][0] += TypeA( x );
		p_matrix[3][1] += TypeA( y );
		p_matrix[3][2] += TypeA( z );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::translate( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_translation )
	{
		return MtxUtils::translate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_translate( SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
	{
		std::memset( p_matrix.ptr(), 0, sizeof( TypeA ) * 4 * 4 );
		TypeA l_unit( 1 );
		p_matrix[0][0] = l_unit;
		p_matrix[1][1] = l_unit;
		p_matrix[2][2] = l_unit;
		p_matrix[3][0] = TypeA( x );
		p_matrix[3][1] = TypeA( y );
		p_matrix[3][2] = TypeA( z );
		p_matrix[3][3] = l_unit;
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_translate( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_translation )
	{
		return set_translate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
	}

	template< typename TypeA, typename TypeB >
	void MtxUtils::get_translate( SquareMatrix< TypeA, 4 > const & p_matrix, Point< TypeB, 3 > & p_translation )
	{
		p_translation[0] = Policy< TypeB >::convert( p_matrix[3][0] );
		p_translation[1] = Policy< TypeB >::convert( p_matrix[3][1] );
		p_translation[2] = Policy< TypeB >::convert( p_matrix[3][2] );
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_transform_rh( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_ptPosition, Point< TypeB, 3 > const & p_ptScale, Quaternion const & p_qOrientation )
	{
		// Ordering:
		//    1. Scale
		//    2. Rotate
		//    3. Translate
		MtxUtils::set_scale( p_matrix, p_ptScale );
		MtxUtils::rotate( p_matrix, p_qOrientation );
		MtxUtils::translate( p_matrix, p_ptPosition );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::set_transform_lh( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_ptPosition, Point< TypeB, 3 > const & p_ptScale, Quaternion const & p_qOrientation )
	{
		return switch_hand( set_transform_rh( p_matrix, p_ptPosition, p_ptScale, p_qOrientation ) );
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::perspective_rh( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_aFOVY, TypeB aspect, TypeB zNear, TypeB zFar )
	{
		TypeA range = Policy< TypeA >::convert( ( 1 / tan( p_aFOVY.Radians() / 2 ) ) );
		p_matrix.initialise();
		p_matrix[0][0] = Policy< TypeA >::convert( range / aspect );
		p_matrix[1][1] = Policy< TypeA >::convert( range );
		p_matrix[2][2] = Policy< TypeA >::convert( - ( zFar + zNear ) / ( zFar - zNear ) );
		p_matrix[2][3] = Policy< TypeA >::convert( - 1 );
		p_matrix[3][2] = Policy< TypeA >::convert( - 2 * zFar * zNear / ( zFar - zNear ) );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::perspective_lh( SquareMatrix< TypeA, 4 > & p_matrix, Angle const & p_aFOVY, TypeB aspect, TypeB zNear, TypeB zFar )
	{
		TypeA range = Policy< TypeA >::convert( ( 1 / tan( p_aFOVY.Radians() / 2 ) ) );
		p_matrix.initialise();
		p_matrix[0][0] = Policy< TypeA >::convert( range / aspect );
		p_matrix[1][1] = Policy< TypeA >::convert( range );
		p_matrix[2][2] = Policy< TypeA >::convert( zFar / ( zNear - zFar ) );
		p_matrix[2][3] = Policy< TypeA >::convert( - 1 );
		p_matrix[3][2] = Policy< TypeA >::convert( zNear * zFar / ( zNear - zFar ) );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::centered_ortho( SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB top, TypeB bottom )
	{
		p_matrix.set_identity();
		p_matrix[0][0] = Policy< TypeA >::convert( 2 / ( right - left ) );
		p_matrix[1][1] = Policy< TypeA >::convert( 2 / ( top - bottom ) );
		p_matrix[0][3] = Policy< TypeA >::convert( -( right + left ) / ( right - left ) );
		p_matrix[1][3] = Policy< TypeA >::convert( -( top + bottom ) / ( top - bottom ) );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::ortho_rh( SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar )
	{
		p_matrix.set_identity();
		p_matrix[0][0] = Policy< TypeA >::convert( 2 / ( right - left ) );
		p_matrix[1][1] = Policy< TypeA >::convert( 2 / ( top - bottom ) );
		p_matrix[2][2] = Policy< TypeA >::convert( -2 / ( zFar - zNear ) );
		p_matrix[3][0] = Policy< TypeA >::convert( -( right + left ) / ( right - left ) );
		p_matrix[3][1] = Policy< TypeA >::convert( -( top + bottom ) / ( top - bottom ) );
		p_matrix[3][2] = Policy< TypeA >::convert( -( zFar + zNear ) / ( zFar - zNear ) );
		p_matrix[3][3] = 1;
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::ortho_lh( SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar )
	{
		p_matrix.set_identity();
		p_matrix[0][0] = Policy< TypeA >::convert( 2 / ( right - left ) );
		p_matrix[1][1] = Policy< TypeA >::convert( 2 / ( top - bottom ) );
		p_matrix[2][2] = Policy< TypeA >::convert( 1 / ( zNear - zFar ) );
		p_matrix[3][0] = Policy< TypeA >::convert( ( left + right ) / ( left - right ) );
		p_matrix[3][1] = Policy< TypeA >::convert( ( top + bottom ) / ( bottom - top ) );
		p_matrix[3][2] = Policy< TypeA >::convert( zNear / ( zNear - zFar ) );
		p_matrix[3][3] = 1;
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::frustum_rh( SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB nearVal, TypeB farVal )
	{
		p_matrix.initialise();
		p_matrix[0][0] = Policy< TypeA >::convert( ( 2 * nearVal ) / ( right - left ) );
		p_matrix[1][1] = Policy< TypeA >::convert( ( 2 * nearVal ) / ( top - bottom ) );
		p_matrix[2][0] = Policy< TypeA >::convert( ( right + left ) / ( right - left ) );
		p_matrix[2][1] = Policy< TypeA >::convert( ( top + bottom ) / ( top - bottom ) );
		p_matrix[2][2] = Policy< TypeA >::convert( ( nearVal + farVal ) / ( nearVal - farVal ) );
		p_matrix[2][3] = Policy< TypeA >::convert( -1 );
		p_matrix[3][2] = Policy< TypeA >::convert( ( 2 * farVal * nearVal ) / ( nearVal - farVal ) );
		return p_matrix;
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > & MtxUtils::look_at( SquareMatrix< TypeA, 4 > & p_matrix, Point< TypeB, 3 > const & p_ptEye, Point< TypeB, 3 > const & p_ptCenter, Point< TypeB, 3 > const & p_ptUp )
	{
		Point< TypeA, 3 > l_f = point::normalise( p_ptCenter - p_ptEye );
		Point< TypeA, 3 > l_u = point::normalise( p_ptUp );
		Point< TypeA, 3 > l_s = point::normalise( l_f ^ l_u );
		l_u = l_s ^ l_f;
		p_matrix.initialise();
		p_matrix[0][0] =  l_s.x;
		p_matrix[1][0] =  l_s.y;
		p_matrix[2][0] =  l_s.z;
		p_matrix[0][1] =  l_u.x;
		p_matrix[1][1] =  l_u.y;
		p_matrix[2][1] =  l_u.z;
		p_matrix[0][2] = -l_f.x;
		p_matrix[1][2] = -l_f.y;
		p_matrix[2][2] = -l_f.z;
		p_matrix[3][0] = -point::dot( l_s, p_ptEye );
		p_matrix[3][1] = -point::dot( l_u, p_ptEye );
		p_matrix[3][2] =  point::dot( l_f, p_ptEye );
		return p_matrix;
	}

	template <typename TypeA, typename TypeB, uint32_t Count>
	Point<TypeB, Count> MtxUtils::mult( SquareMatrix< TypeA, 4 > const & p_matrix, Point<TypeB, Count> const & p_vertex )
	{
		return MtxMultiplicator< TypeA, TypeB, Count >()( p_matrix, p_vertex );
	}

	template< typename TypeA, typename TypeB >
	SquareMatrix< TypeA, 4 > MtxUtils::mult( SquareMatrix< TypeA, 4 > const & p_matrixA, SquareMatrix<TypeB, 4> const & p_matrixB )
	{
		return p_matrixA.multiply( p_matrixB );
	}
	template< typename TypeA >
	SquareMatrix< TypeA, 4 > & MtxUtils::switch_hand( SquareMatrix< TypeA, 4 > const & p_matrix )
	{
		auto l_col2 = p_matrix[2];
		auto l_col3 = p_matrix[3];
		std::swap( l_col2[0], l_col3[0] );
		std::swap( l_col2[1], l_col3[1] );
		std::swap( l_col2[2], l_col3[2] );
		std::swap( l_col2[3], l_col3[3] );
		return p_matrix;
	}
	template< typename TypeA >
	SquareMatrix< TypeA, 4 > MtxUtils::get_switch_hand( SquareMatrix< TypeA, 4 > const & p_matrix )
	{
		SquareMatrix< TypeA, 4 > l_mtxReturn( p_matrix );
		switch_hand( l_mtxReturn );
		return l_mtxReturn;
	}
}
