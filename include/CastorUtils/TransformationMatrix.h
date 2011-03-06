/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_TransformationMatrix___
#define ___Castor_TransformationMatrix___

#include "Matrix.h"
#include "Point.h"
#include "Angle.h"
#include "Quaternion.h"

namespace Castor
{	namespace Math
{	namespace MtxUtils
{
	template <typename TypeA, typename TypeB>
	void rotate			( SquareMatrix<TypeA, 4> & p_matrix, const Angle & p_angle, const Point<TypeB, 3> & p_axis);
	template <typename TypeA>
	void rotate			( SquareMatrix<TypeA, 4> & p_matrix, const Quaternion & p_quat);
	template <typename TypeA>
	void yaw			( SquareMatrix<TypeA, 4> & p_matrix, const Angle & p_angle);
	template <typename TypeA>
	void pitch			( SquareMatrix<TypeA, 4> & p_matrix, const Angle & p_angle);
	template <typename TypeA>
	void roll			( SquareMatrix<TypeA, 4> & p_matrix, const Angle & p_angle);
	template <typename TypeA, typename TypeB>
	void scale			( SquareMatrix<TypeA, 4> & p_matrix, TypeB p_hx, TypeB p_hy, TypeB p_hz);
	template <typename TypeA, typename TypeB>
	void scale			( SquareMatrix<TypeA, 4> & p_matrix, const Point<TypeB, 3> & p_scale);
	template <typename TypeA, typename TypeB>
	void translate		( SquareMatrix<TypeA, 4> & p_matrix, TypeB p_tx, TypeB p_ty, TypeB p_tz);
	template <typename TypeA, typename TypeB>
	void translate		( SquareMatrix<TypeA, 4> & p_matrix, const Point<TypeB, 3> & p_translation);
	template <typename TypeA, typename TypeB>
	void perspective	( SquareMatrix<TypeA, 4> & p_matrix, TypeB fovy, TypeB aspect, TypeB zNear, TypeB zFar);
	template <typename TypeA, typename TypeB>
	void ortho			( SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar);
	template <typename TypeA, typename TypeB>
	void ortho			( SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB top, TypeB bottom);
	template <typename TypeA, typename TypeB>
	void frustum		( SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB nearVal, TypeB farVal);
	template <typename TypeA, typename TypeB, size_t Count>
	Point<TypeB, Count>		mult( const SquareMatrix<TypeA, 4> & p_matrix, const Point<TypeB, Count> & p_vertex);
	template <typename TypeA, typename TypeB>
	SquareMatrix<TypeA, 4>	mult( const SquareMatrix<TypeA, 4> & p_matrixA, const SquareMatrix<TypeB, 4> & p_matrixB);
	template <typename TypeA>
	SquareMatrix<TypeA, 4> switch_hand	( const SquareMatrix<TypeA, 4> & p_matrix);
}
}
}

#include "TransformationMatrix.inl"

#endif
