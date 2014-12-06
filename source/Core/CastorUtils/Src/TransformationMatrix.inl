template< typename TypeA, typename TypeB >
void Castor::MtxUtils::rotate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Angle const & p_angle, Castor::Point< TypeB, 3 > const & p_axis )
{
	Castor::MtxUtils::rotate( p_matrix, Castor::Quaternion( p_axis, p_angle ) );
}

template< typename TypeA >
void Castor::MtxUtils::rotate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Quaternion const & p_quat )
{
	/*
		Point< TypeA, 3 > l_axis;
		Angle l_angle;
		TypeA c;
		TypeA s;

		Castor::SquareMatrix< TypeA, 4 > l_matrix;
		p_quat.ToAxisAngle( l_axis, l_angle );
		c = Policy< TypeA >::convert( l_angle.Cos() );
		s = Policy< TypeA >::convert( l_angle.Sin() );

		TypeA x = l_axis.at( 0 );
		TypeA y = l_axis.at( 1 );
		TypeA z = l_axis.at( 2 );

		TypeA xx	= x * x;
		TypeA xy	= x * y;
		TypeA xz	= x * z;

		TypeA yy	= y * y;
		TypeA yz	= y * z;

		TypeA zz	= z * z;

		TypeA xs	= x * s;
		TypeA ys	= y * s;
		TypeA zs	= z * s;

		l_matrix[0][0] = Policy< TypeA >::convert( xx * (1.0 - c) + c  );
		l_matrix[1][0] = Policy< TypeA >::convert( xy * (1.0 - c) - zs );
		l_matrix[2][0] = Policy< TypeA >::convert( xz * (1.0 - c) + ys );
		l_matrix[3][0] = Policy< TypeA >::convert( 0.0 );

		l_matrix[0][1] = Policy< TypeA >::convert( xy * (1.0 - c) + zs );
		l_matrix[1][1] = Policy< TypeA >::convert( yy * (1.0 - c) + c  );
		l_matrix[2][1] = Policy< TypeA >::convert( yz * (1.0 - c) - xs );
		l_matrix[3][1] = Policy< TypeA >::convert( 0.0 );

		l_matrix[0][2] = Policy< TypeA >::convert( xz * (1.0 - c) - ys );
		l_matrix[1][2] = Policy< TypeA >::convert( yz * (1.0 - c) + xs );
		l_matrix[2][2] = Policy< TypeA >::convert( zz * (1.0 - c) + c  );
		l_matrix[3][2] = Policy< TypeA >::convert( 0.0 );

		l_matrix[0][3] = Policy< TypeA >::convert( 0.0 );
		l_matrix[1][3] = Policy< TypeA >::convert( 0.0 );
		l_matrix[2][3] = Policy< TypeA >::convert( 0.0 );
		l_matrix[3][3] = Policy< TypeA >::convert( 1.0 );
	/**/
	Castor::SquareMatrix< TypeA, 4 > l_matrix;
	p_quat.ToRotationMatrix( l_matrix );
	p_matrix *= l_matrix;
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::set_rotate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Angle const & p_angle, Castor::Point< TypeB, 3 > const & p_axis )
{
	Castor::MtxUtils::set_rotate( p_matrix, Castor::Quaternion( p_axis, p_angle ) );
}

template< typename TypeA >
void Castor::MtxUtils::set_rotate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Quaternion const & p_quat )
{
	p_quat.ToRotationMatrix( p_matrix );
}

template< typename TypeA >
void Castor::MtxUtils::get_rotate( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Quaternion & p_quat )
{
	p_quat.FromRotationMatrix( p_matrix );
}

template< typename TypeA >
void Castor::MtxUtils::yaw( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Angle const & p_angle )
{
	Castor::MtxUtils::rotate( p_matrix, p_angle, Castor::Point< TypeA, 3 >( 0, 1, 0 ) );
}

template< typename TypeA >
void Castor::MtxUtils::pitch( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Angle const & p_angle )
{
	Castor::MtxUtils::rotate( p_matrix, p_angle, Castor::Point< TypeA, 3 >( 0, 0, 1 ) );
}

template< typename TypeA >
void Castor::MtxUtils::roll( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Angle const & p_angle )
{
	Castor::MtxUtils::rotate( p_matrix, p_angle, Castor::Point< TypeA, 3 >( 1, 0, 0 ) );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::scale( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
{
	p_matrix[0][0] *= Policy< TypeA >::convert( x );
	p_matrix[1][1] *= Policy< TypeA >::convert( y );
	p_matrix[2][2] *= Policy< TypeA >::convert( z );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::scale( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_scale )
{
	Castor::MtxUtils::scale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::set_scale( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
{
	p_matrix[0][0] = Policy< TypeA >::convert( x );
	p_matrix[0][1] = TypeA( 0 );
	p_matrix[0][2] = TypeA( 0 );
	p_matrix[0][3] = TypeA( 0 );
	p_matrix[1][0] = TypeA( 0 );
	p_matrix[1][1] = Policy< TypeA >::convert( y );
	p_matrix[1][2] = TypeA( 0 );
	p_matrix[1][3] = TypeA( 0 );
	p_matrix[2][0] = TypeA( 0 );
	p_matrix[2][1] = TypeA( 0 );
	p_matrix[2][2] = Policy< TypeA >::convert( z );
	p_matrix[2][3] = TypeA( 0 );
	p_matrix[3][0] = TypeA( 0 );
	p_matrix[3][1] = TypeA( 0 );
	p_matrix[3][2] = TypeA( 0 );
	p_matrix[3][3] = TypeA( 1 );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::set_scale( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_scale )
{
	Castor::MtxUtils::set_scale( p_matrix, p_scale[0], p_scale[1], p_scale[2] );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::get_scale( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 3 > & p_scale )
{
	p_scale[0] = Policy< TypeB >::convert( p_matrix[0][0] );
	p_scale[1] = Policy< TypeB >::convert( p_matrix[1][1] );
	p_scale[2] = Policy< TypeB >::convert( p_matrix[2][2] );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::translate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
{
	Castor::SquareMatrix< TypeA, 4 > l_matrix;
	l_matrix.set_identity();
	l_matrix[3][0] = TypeA( x );
	l_matrix[3][1] = TypeA( y );
	l_matrix[3][2] = TypeA( z );
	p_matrix = p_matrix * l_matrix;
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::translate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_translation )
{
	Castor::MtxUtils::translate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::set_translate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB x, TypeB y, TypeB z )
{
	p_matrix[0][0] = TypeA( 1 );
	p_matrix[0][1] = TypeA( 0 );
	p_matrix[0][2] = TypeA( 0 );
	p_matrix[0][3] = TypeA( 0 );
	p_matrix[1][0] = TypeA( 0 );
	p_matrix[1][1] = TypeA( 1 );
	p_matrix[1][2] = TypeA( 0 );
	p_matrix[1][3] = TypeA( 0 );
	p_matrix[2][0] = TypeA( 0 );
	p_matrix[2][1] = TypeA( 0 );
	p_matrix[2][2] = TypeA( 1 );
	p_matrix[2][3] = TypeA( 0 );
	p_matrix[3][0] = Policy< TypeA >::convert( x );
	p_matrix[3][1] = Policy< TypeA >::convert( y );
	p_matrix[3][2] = Policy< TypeA >::convert( z );
	p_matrix[3][3] = TypeA( 1 );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::set_translate( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_translation )
{
	Castor::MtxUtils::set_translate( p_matrix, p_translation[0], p_translation[1], p_translation[2] );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::get_translate( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 3 > & p_translation )
{
	p_translation[0] = Policy< TypeB >::convert( p_matrix[3][0] );
	p_translation[1] = Policy< TypeB >::convert( p_matrix[3][1] );
	p_translation[2] = Policy< TypeB >::convert( p_matrix[3][2] );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::set_transform( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_ptPosition, Castor::Point< TypeB, 3 > const & p_ptScale, Castor::Quaternion const & p_qOrientation )
{
	// Ordering:
	//    1. Scale
	//    2. Rotate
	//    3. Translate
	p_qOrientation.ToRotationMatrix( p_matrix );
	// Set up final matrix with scale, rotation and translation
	p_matrix[0][0] *= p_ptScale[0];
	p_matrix[0][1] *= p_ptScale[0];
	p_matrix[0][2] *= p_ptScale[0];
	p_matrix[1][0] *= p_ptScale[1];
	p_matrix[1][1] *= p_ptScale[1];
	p_matrix[1][2] *= p_ptScale[1];
	p_matrix[2][0] *= p_ptScale[2];
	p_matrix[2][1] *= p_ptScale[2];
	p_matrix[2][2] *= p_ptScale[2];
	p_matrix[3][0] = p_ptPosition[0];
	p_matrix[3][1] = p_ptPosition[1];
	p_matrix[3][2] = p_ptPosition[2];
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::perspective( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Angle const & p_aFOVY, TypeB aspect, TypeB zNear, TypeB zFar )
{
	/*
		TypeA range = Policy< TypeA >::convert( tan( p_aFOVY.Radians() / 2) * zNear);
		TypeA left = -range * Policy< TypeA >::convert( aspect);
		TypeA right = range * Policy< TypeA >::convert( aspect);
		TypeA bottom = -range;
		TypeA top = range;

		p_matrix.initialise();
		p_matrix[0][0] = Policy< TypeA >::convert( 2 * zNear / (right - left));
		p_matrix[1][1] = Policy< TypeA >::convert( 2 * zNear / (top - bottom));
		p_matrix[2][2] = Policy< TypeA >::convert( - (zFar + zNear) / (zFar - zNear));
		p_matrix[2][3] = Policy< TypeA >::convert( - 1);
		p_matrix[3][2] = Policy< TypeA >::convert( - 2 * zFar * zNear / (zFar - zNear));
	*/
	TypeA range = Policy< TypeA >::convert( ( 1 / tan( p_aFOVY.Radians() / 2 ) ) );
	p_matrix.initialise();
	p_matrix[0][0] = Policy< TypeA >::convert( range / aspect );
	p_matrix[1][1] = Policy< TypeA >::convert( range );
	p_matrix[2][2] = Policy< TypeA >::convert( - ( zFar + zNear ) / ( zFar - zNear ) );
	p_matrix[2][3] = Policy< TypeA >::convert( - 1 );
	p_matrix[3][2] = Policy< TypeA >::convert( - 2 * zFar * zNear / ( zFar - zNear ) );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::perspective_dx( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Angle const & p_aFOVY, TypeB aspect, TypeB zNear, TypeB zFar )
{
	TypeA range = Policy< TypeA >::convert( ( 1 / tan( p_aFOVY.Radians() / 2 ) ) );
	p_matrix.initialise();
	p_matrix[0][0] = Policy< TypeA >::convert( range / aspect );
	p_matrix[1][1] = Policy< TypeA >::convert( range );
	p_matrix[2][2] = Policy< TypeA >::convert( zFar / ( zNear - zFar ) );
	p_matrix[2][3] = Policy< TypeA >::convert( - 1 );
	p_matrix[3][2] = Policy< TypeA >::convert( zNear * zFar / ( zNear - zFar ) );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::ortho( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar )
{
	p_matrix.set_identity();
	p_matrix[0][0] = Policy< TypeA >::convert( 2 / ( right - left ) );
	p_matrix[1][1] = Policy< TypeA >::convert( 2 / ( top - bottom ) );
	p_matrix[2][2] = Policy< TypeA >::convert( -2 / ( zFar - zNear ) );
	p_matrix[3][0] = Policy< TypeA >::convert( -( right + left ) / ( right - left ) );
	p_matrix[3][1] = Policy< TypeA >::convert( -( top + bottom ) / ( top - bottom ) );
	p_matrix[3][2] = Policy< TypeA >::convert( -( zFar + zNear ) / ( zFar - zNear ) );
	p_matrix[3][3] = 1;
}
template< typename TypeA, typename TypeB >
void Castor::MtxUtils::ortho( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB top, TypeB bottom )
{
	p_matrix.set_identity();
	p_matrix[0][0] = Policy< TypeA >::convert( 2 / ( right - left ) );
	p_matrix[1][1] = Policy< TypeA >::convert( 2 / ( top - bottom ) );
	p_matrix[0][3] = Policy< TypeA >::convert( -( right + left ) / ( right - left ) );
	p_matrix[1][3] = Policy< TypeA >::convert( -( top + bottom ) / ( top - bottom ) );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::ortho_dx( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar )
{
	p_matrix.set_identity();
	p_matrix[0][0] = Policy< TypeA >::convert( 2 / ( right - left ) );
	p_matrix[1][1] = Policy< TypeA >::convert( 2 / ( top - bottom ) );
	p_matrix[2][2] = Policy< TypeA >::convert( 1 / ( zNear - zFar ) );
	p_matrix[3][0] = Policy< TypeA >::convert( ( left + right ) / ( left - right ) );
	p_matrix[3][1] = Policy< TypeA >::convert( ( top + bottom ) / ( bottom - top ) );
	p_matrix[3][2] = Policy< TypeA >::convert( zNear / ( zNear - zFar ) );
	p_matrix[3][3] = 1;
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::frustum( Castor::SquareMatrix< TypeA, 4 > & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB nearVal, TypeB farVal )
{
	p_matrix.initialise();
	p_matrix[0][0] = Policy< TypeA >::convert( ( 2 * nearVal ) / ( right - left ) );
	p_matrix[1][1] = Policy< TypeA >::convert( ( 2 * nearVal ) / ( top - bottom ) );
	p_matrix[2][0] = Policy< TypeA >::convert( ( right + left ) / ( right - left ) );
	p_matrix[2][1] = Policy< TypeA >::convert( ( top + bottom ) / ( top - bottom ) );
	p_matrix[2][2] = Policy< TypeA >::convert( ( nearVal + farVal ) / ( nearVal - farVal ) );
	p_matrix[2][3] = Policy< TypeA >::convert( -1 );
	p_matrix[3][2] = Policy< TypeA >::convert( ( 2 * farVal * nearVal ) / ( nearVal - farVal ) );
}

template< typename TypeA, typename TypeB >
void Castor::MtxUtils::look_at( Castor::SquareMatrix< TypeA, 4 > & p_matrix, Castor::Point< TypeB, 3 > const & p_ptEye, Castor::Point< TypeB, 3 > const & p_ptCenter, Castor::Point< TypeB, 3 > const & p_ptUp )
{
	Castor::Point< TypeA, 3 > l_f = point::normalise( p_ptCenter - p_ptEye );
	Castor::Point< TypeA, 3 > l_u = point::normalise( p_ptUp );
	Castor::Point< TypeA, 3 > l_s = point::normalise( l_f ^ l_u );
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
}

template <typename TypeA, typename TypeB, uint32_t Count>
Castor::Point<TypeB, Count> Castor::MtxUtils::mult( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point<TypeB, Count> const & p_vertex )
{
	return MtxMultiplicator< TypeA, TypeB, Count >()( p_matrix, p_vertex );
}

template< typename TypeA, typename TypeB >
Castor::SquareMatrix< TypeA, 4 > Castor::MtxUtils::mult( Castor::SquareMatrix< TypeA, 4 > const & p_matrixA, Castor::SquareMatrix<TypeB, 4> const & p_matrixB )
{
	return p_matrixA.multiply( p_matrixB );
}
template< typename TypeA >
Castor::SquareMatrix< TypeA, 4 > Castor::MtxUtils::switch_hand( SquareMatrix< TypeA, 4 > const & p_matrix )
{
	SquareMatrix< TypeA, 4 > l_mtxReturn( p_matrix );
	typename SquareMatrix< TypeA, 4 >::row_type l_row = l_mtxReturn.get_row( 2 ) * -1;
	l_mtxReturn.set_row( 2, l_row );
	return l_mtxReturn;
}
