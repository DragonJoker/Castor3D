template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: rotate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Angle const & p_angle, Castor::Math::Point<TypeB, 3> const & p_axis)
{
	Castor::Math::MtxUtils::rotate( p_matrix, Castor::Math::Quaternion( p_angle.Radians(), Policy<real>::convert( p_axis[0]), Policy<real>::convert( p_axis[1]), Policy<real>::convert( p_axis[2])));
}

template <typename TypeA>
void Castor::Math::MtxUtils :: rotate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Quaternion const & p_quat)
{
	real x = p_quat[0];
	real y = p_quat[1];
	real z = p_quat[2];
	real w = p_quat[3];

	p_matrix[0][0] = Policy<TypeA>::convert( 1.0 - 2.0 * ( y * y + z * z));
	p_matrix[0][1] = Policy<TypeA>::convert( 2.0 * ( x * y - z * w ));
	p_matrix[0][2] = Policy<TypeA>::convert( 2.0 * ( x * z + y * w ));
	p_matrix[0][3] = 0;

	p_matrix[1][0] = Policy<TypeA>::convert( 2.0 * (x * y + z * w));
	p_matrix[1][1] = Policy<TypeA>::convert( 1.0 - 2.0 * ( x * x + z * z));
	p_matrix[1][2] = Policy<TypeA>::convert( 2.0 * ( y * z - x * w ));
	p_matrix[1][3] = 0;

	p_matrix[2][0] = Policy<TypeA>::convert( 2.0 * (x * z - y * w));
	p_matrix[2][1] = Policy<TypeA>::convert( 2.0 * (z * y + x * w ));
	p_matrix[2][2] = Policy<TypeA>::convert( 1.0 - 2.0 * ( x * x + y * y ));
	p_matrix[2][3] = 0;

	p_matrix[3][0] = 0;
	p_matrix[3][1] = 0;
	p_matrix[3][2] = 0;
	p_matrix[3][3] = 1;
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: set_rotate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Angle const & p_angle, Castor::Math::Point<TypeB, 3> const & p_axis)
{
	Castor::Math::MtxUtils::set_rotate( p_matrix, Castor::Math::Quaternion( p_angle.Radians(), Policy<real>::convert( p_axis[0]), Policy<real>::convert( p_axis[1]), Policy<real>::convert( p_axis[2])));
}

template <typename TypeA>
void Castor::Math::MtxUtils :: set_rotate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Quaternion const & p_quat)
{
	real x = p_quat[0];
	real y = p_quat[1];
	real z = p_quat[2];
	real w = p_quat[3];

	p_matrix[0][0] = Policy<TypeA>::convert( 1.0 - 2.0 * ( y * y + z * z));
	p_matrix[0][1] = Policy<TypeA>::convert( 2.0 * ( x * y - z * w ));
	p_matrix[0][2] = Policy<TypeA>::convert( 2.0 * ( x * z + y * w ));
	p_matrix[0][3] = 0;

	p_matrix[1][0] = Policy<TypeA>::convert( 2.0 * (x * y + z * w));
	p_matrix[1][1] = Policy<TypeA>::convert( 1.0 - 2.0 * ( x * x + z * z));
	p_matrix[1][2] = Policy<TypeA>::convert( 2.0 * ( y * z - x * w ));
	p_matrix[1][3] = 0;

	p_matrix[2][0] = Policy<TypeA>::convert( 2.0 * (x * z - y * w));
	p_matrix[2][1] = Policy<TypeA>::convert( 2.0 * (z * y + x * w ));
	p_matrix[2][2] = Policy<TypeA>::convert( 1.0 - 2.0 * ( x * x + y * y ));
	p_matrix[2][3] = 0;

	p_matrix[3][0] = 0;
	p_matrix[3][1] = 0;
	p_matrix[3][2] = 0;
	p_matrix[3][3] = 1;
}

template <typename TypeA>
void Castor::Math::MtxUtils :: yaw( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Angle const & p_angle)
{
	Castor::Math::MtxUtils::rotate( p_matrix, p_angle, Castor::Math::Point<TypeA, 3>( 0, 1, 0));
}

template <typename TypeA>
void Castor::Math::MtxUtils :: pitch( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Angle const & p_angle)
{
	Castor::Math::MtxUtils::rotate( p_matrix, p_angle, Castor::Math::Point<TypeA, 3>( 0, 0, 1));
}

template <typename TypeA>
void Castor::Math::MtxUtils :: roll( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Angle const & p_angle)
{
	Castor::Math::MtxUtils::rotate( p_matrix, p_angle, Castor::Math::Point<TypeA, 3>( 1, 0, 0));
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: scale( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, TypeB x, TypeB y, TypeB z)
{
	p_matrix[0] = p_matrix[0] * Policy<TypeA>::convert( x);
	p_matrix[1] = p_matrix[1] * Policy<TypeA>::convert( y);
	p_matrix[2] = p_matrix[2] * Policy<TypeA>::convert( z);
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: scale( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Point<TypeB, 3> const & p_scale)
{
	Castor::Math::MtxUtils::scale( p_matrix, p_scale[0], p_scale[1], p_scale[2]);
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: set_scale( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, TypeB x, TypeB y, TypeB z)
{
	p_matrix[0].copy( Point<TypeA, 4>( Policy<TypeA>::convert( x), TypeA( 0), TypeA( 0), TypeA( 0)));
	p_matrix[1].copy( Point<TypeA, 4>( TypeA( 0), Policy<TypeA>::convert( y), TypeA( 0), TypeA( 0)));
	p_matrix[2].copy( Point<TypeA, 4>( TypeA( 0), TypeA( 0), Policy<TypeA>::convert( z), TypeA( 0)));
	p_matrix[3].copy( Point<TypeA, 4>( TypeA( 0), TypeA( 0), TypeA( 0), TypeA( 0)));
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: set_scale( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Point<TypeB, 3> const & p_scale)
{
	Castor::Math::MtxUtils::set_scale( p_matrix, p_scale[0], p_scale[1], p_scale[2]);
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: translate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, TypeB x, TypeB y, TypeB z)
{
	p_matrix[3] = p_matrix[0] * Policy<TypeA>::convert( x)
				+ p_matrix[1] * Policy<TypeA>::convert( y)
				+ p_matrix[2] * Policy<TypeA>::convert( z)
				+ p_matrix[3];
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: translate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Point<TypeB, 3> const & p_translation)
{
	Castor::Math::MtxUtils::translate( p_matrix, p_translation[0], p_translation[1], p_translation[2]);
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: set_translate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, TypeB x, TypeB y, TypeB z)
{
	p_matrix[0].copy( Point<TypeA, 4>( TypeA( 1), TypeA( 0), TypeA( 0), TypeA( 0)));
	p_matrix[1].copy( Point<TypeA, 4>( TypeA( 0), TypeA( 1), TypeA( 0), TypeA( 0)));
	p_matrix[2].copy( Point<TypeA, 4>( TypeA( 0), TypeA( 0), TypeA( 1), TypeA( 0)));
	p_matrix[3].copy( Point<TypeA, 4>( Policy<TypeA>::convert( x), Policy<TypeA>::convert( y), Policy<TypeA>::convert( z), TypeA( 0)));
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: set_translate( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Point<TypeB, 3> const & p_translation)
{
	Castor::Math::MtxUtils::set_translate( p_matrix, p_translation[0], p_translation[1], p_translation[2]);
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: perspective( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, Castor::Math::Angle const & p_aFOVY, TypeB aspect, TypeB zNear, TypeB zFar)
{
	TypeA range = Policy<TypeA>::convert( tan( p_aFOVY.Radians() / 2) * zNear);
	TypeA left = -range * Policy<TypeA>::convert( aspect);
	TypeA right = range * Policy<TypeA>::convert( aspect);
	TypeA bottom = -range;
	TypeA top = range;

	p_matrix.Init();
	p_matrix[0][0] = Policy<TypeA>::convert( 2 * zNear / (right - left));
	p_matrix[1][1] = Policy<TypeA>::convert( 2 * zNear / (top - bottom));
	p_matrix[2][2] = Policy<TypeA>::convert( - (zFar + zNear) / (zFar - zNear));
	p_matrix[2][3] = Policy<TypeA>::convert( - 1);
	p_matrix[3][2] = Policy<TypeA>::convert( - 2 * zFar * zNear / (zFar - zNear));
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: ortho( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar)
{
	p_matrix.SetIdentity();
	p_matrix[0][0] = Policy<TypeA>::convert( 2 / (right - left));
	p_matrix[1][1] = Policy<TypeA>::convert( 2 / (top - bottom));
	p_matrix[2][2] = Policy<TypeA>::convert( 2 / (zNear - zFar));
	p_matrix[0][3] = Policy<TypeA>::convert( (left + right) / (left - right));
	p_matrix[1][3] = Policy<TypeA>::convert( (bottom + top) / (bottom - top));
	p_matrix[2][3] = Policy<TypeA>::convert( (zNear + zFar) / (zNear - zFar));
	p_matrix[3][3] = -1;
}
template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: ortho( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB top, TypeB bottom)
{
	p_matrix.SetIdentity();
	p_matrix[0][0] = Policy<TypeA>::convert(   2 / (right - left));
	p_matrix[1][1] = Policy<TypeA>::convert(   2 / (top - bottom));
	p_matrix[0][3] = Policy<TypeA>::convert( (left + right) / (left - right));
	p_matrix[1][3] = Policy<TypeA>::convert( (bottom + top) / (bottom - top));
}

template <typename TypeA, typename TypeB>
void Castor::Math::MtxUtils :: frustum( Castor::Math::SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB nearVal, TypeB farVal)
{
	p_matrix.Init();
	p_matrix[0][0] = Policy<TypeA>::convert( (2 * nearVal) / (right - left));
	p_matrix[1][1] = Policy<TypeA>::convert( (2 * nearVal) / (top - bottom));
	p_matrix[2][0] = Policy<TypeA>::convert( (right + left) / (right - left));
	p_matrix[2][1] = Policy<TypeA>::convert( (top + bottom) / (top - bottom));
	p_matrix[2][2] = Policy<TypeA>::convert( (nearVal + farVal) / (nearVal - farVal));
	p_matrix[2][3] = Policy<TypeA>::convert( -1);
	p_matrix[3][2] = Policy<TypeA>::convert( (2 * farVal * nearVal) / (nearVal - farVal));
}

template <typename TypeA, typename TypeB, size_t Count>
Castor::Math::Point<TypeB, Count> Castor::Math::MtxUtils :: mult( Castor::Math::SquareMatrix<TypeA, 4> const & p_matrix, Castor::Math::Point<TypeB, Count> const & p_vertex)
{
	Castor::Math::Point<TypeB, Count> l_ptReturn;

	for (size_t i = 0 ; i < Count ; i++)
	{
		l_ptReturn[i] = p_matrix[0][i] * p_vertex[0];

		if (Count > 1)
		{
			l_ptReturn[i] += p_matrix[1][i] * p_vertex[1];

			if (Count > 2)
			{
				l_ptReturn[i] += p_matrix[2][i] * p_vertex[2];

				if (Count > 3)
				{
					l_ptReturn[i] += p_matrix[3][i] * p_vertex[3];
				}
				else
				{
					l_ptReturn[i] += p_matrix[3][i];
				}
			}
			else
			{
				l_ptReturn[i] += p_matrix[2][i];
			}
		}
		else
		{
			l_ptReturn[i] += p_matrix[1][i];
		}
	}

	return l_ptReturn;
}

template <typename TypeA, typename TypeB>
Castor::Math::SquareMatrix<TypeA, 4> Castor::Math::MtxUtils :: mult( Castor::Math::SquareMatrix<TypeA, 4> const & p_matrixA, Castor::Math::SquareMatrix<TypeB, 4> const & p_matrixB)
{
	return p_matrixA.Multiply( p_matrixB);
}
template <typename TypeA>
Castor::Math::SquareMatrix<TypeA, 4> Castor::Math::MtxUtils :: switch_hand( SquareMatrix<TypeA, 4> const & p_matrix)
{
	SquareMatrix<TypeA, 4> l_mtxReturn( p_matrix);
	typename SquareMatrix<TypeA, 4>::row_type l_row = l_mtxReturn.GetRow( 2) * -1;
	l_mtxReturn.SetRow( 2, l_row);
	return l_mtxReturn;
}
