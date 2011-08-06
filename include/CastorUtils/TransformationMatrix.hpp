/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "Matrix.hpp"
#include "Point.hpp"
#include "Angle.hpp"
#include "Quaternion.hpp"

namespace Castor
{	namespace Math
{
	//! 4x4 Matrix basic operations
	/*!
	\author Sylvain DOREMUS
	Holds rotation, translation, scale, multiplication operations over 4x4 matrix.
	Also computes frustum, perspective, ortho, switches left-hand to right-hand and reciprocally
	*/
	class MtxUtils
	{
	public:
		/**
		 * Rotation function, builds a rotation matrix from the angle and the axis in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the rotation matrix
		 *@param p_angle : [in] The rotation angle
		 *@param p_axis : [in] The axis around which the angle is set
		 */
		template <typename TypeA, typename TypeB>
		static void						rotate			( SquareMatrix<TypeA, 4> & p_matrix, Angle const & p_angle, Point<TypeB, 3> const & p_axis);
		/**
		 * Rotation function, builds a rotation matrix from the quaternion in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the rotation matrix
		 *@param p_quat : [in] The quaterion used to compute the matrix
		 */
		template <typename TypeA>
		static void						rotate			( SquareMatrix<TypeA, 4> & p_matrix, Quaternion const & p_quat);
		/**
		 * Rotation function, builds a rotation matrix from the angle and the axis in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the rotation matrix
		 *@param p_angle : [in] The rotation angle
		 *@param p_axis : [in] The axis around which the angle is set
		 */
		template <typename TypeA, typename TypeB>
		static void						set_rotate		( SquareMatrix<TypeA, 4> & p_matrix, Angle const & p_angle, Point<TypeB, 3> const & p_axis);
		/**
		 * Rotation function, builds a rotation matrix from the quaternion in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the rotation matrix
		 *@param p_quat : [in] The quaterion used to compute the matrix
		 */
		template <typename TypeA>
		static void						set_rotate		( SquareMatrix<TypeA, 4> & p_matrix, Quaternion const & p_quat);
		/**
		 * Rotation function, around Y axis. Builds a rotation matrix from the angle in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the rotation matrix
		 *@param p_angle : [in] The rotation angle
		 */
		template <typename TypeA>
		static void						yaw				( SquareMatrix<TypeA, 4> & p_matrix, Angle const & p_angle);
		/**
		 * Rotation function, around X axis. Builds a rotation matrix from the angle in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the rotation matrix
		 *@param p_angle : [in] The rotation angle
		 */
		template <typename TypeA>
		static void						pitch			( SquareMatrix<TypeA, 4> & p_matrix, Angle const & p_angle);
		/**
		 * Rotation function, around Z axis. Builds a rotation matrix from the angle in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the rotation matrix
		 *@param p_angle : [in] The rotation angle
		 */
		template <typename TypeA>
		static void						roll			( SquareMatrix<TypeA, 4> & p_matrix, Angle const & p_angle);
		/**
		 * Scale function. Scales matrix from the three values in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be scaled
		 *@param p_hx, p_hy, p_hz : [in] The scale values
		 */
		template <typename TypeA, typename TypeB>
		static void						scale			( SquareMatrix<TypeA, 4> & p_matrix, TypeB p_hx, TypeB p_hy, TypeB p_hz);
		/**
		 * Scale function. Scales matrix from the value in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be scaled
		 *@param p_scale : [in] The scale value
		 */
		template <typename TypeA, typename TypeB>
		static void						scale			( SquareMatrix<TypeA, 4> & p_matrix, const Point<TypeB, 3> & p_scale);
		/**
		 * Scale function. Scales matrix from the three values in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be scaled
		 *@param p_hx, p_hy, p_hz : [in] The scale values
		 */
		template <typename TypeA, typename TypeB>
		static void						set_scale		( SquareMatrix<TypeA, 4> & p_matrix, TypeB p_hx, TypeB p_hy, TypeB p_hz);
		/**
		 * Scale function. Scales matrix from the value in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be scaled
		 *@param p_scale : [in] The scale value
		 */
		template <typename TypeA, typename TypeB>
		static void						set_scale		( SquareMatrix<TypeA, 4> & p_matrix, Point<TypeB, 3> const & p_scale);
		/**
		 * Translate function. Translates a matrix from the three values in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be translated
		 *@param p_hx, p_hy, p_hz : [in] The translate values
		 */
		template <typename TypeA, typename TypeB>
		static void						translate		( SquareMatrix<TypeA, 4> & p_matrix, TypeB p_tx, TypeB p_ty, TypeB p_tz);
		/**
		 * Translate function. Translates a matrix from the value in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be translated
		 *@param p_scale : [in] The translate value
		 */
		template <typename TypeA, typename TypeB>
		static void						translate		( SquareMatrix<TypeA, 4> & p_matrix, Point<TypeB, 3> const & p_translation);
		/**
		 * Translate function. Translates a matrix from the three values in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be translated
		 *@param p_hx, p_hy, p_hz : [in] The translate values
		 */
		template <typename TypeA, typename TypeB>
		static void						set_translate	( SquareMatrix<TypeA, 4> & p_matrix, TypeB p_tx, TypeB p_ty, TypeB p_tz);
		/**
		 * Translate function. Translates a matrix from the value in parameter.
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will be translated
		 *@param p_scale : [in] The translate value
		 */
		template <typename TypeA, typename TypeB>
		static void						set_translate	( SquareMatrix<TypeA, 4> & p_matrix, Point<TypeB, 3> const & p_translation);
		/**
		 * Builds a matrix that sets a centered perspective projection from the given parameters
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the perspective matrix
		 *@param fovy : [in] Y Field of View
		 *@param aspect : [in] Width / Height ratio
		 *@param zNear : [in] Near clipping plane value
		 *@param zFar : [in] Far clipping plane value
		 */
		template <typename TypeA, typename TypeB>
		static void						perspective		( SquareMatrix<TypeA, 4> & p_matrix, Angle const & p_aFOVY, TypeB aspect, TypeB zNear, TypeB zFar);
		/**
		 * Builds a matrix that sets a non centered perspective projection from the given parameters
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the perspective matrix
		 *@param left : [in] Left clipping plane value
		 *@param right : [in] Right clipping plane value
		 *@param bottom : [in] Bottom clipping plane value
		 *@param top : [in] Top clipping plane value
		 *@param zNear : [in] Near clipping plane value
		 *@param zFar : [in] Far clipping plane value
		 */
		template <typename TypeA, typename TypeB>
		static void						frustum			( SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB nearVal, TypeB farVal);
		/**
		 * Builds a matrix that sets a centered orthogonal projection from the given parameters
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the perspective matrix
		 *@param left : [in] Left clipping plane value
		 *@param right : [in] Right clipping plane value
		 *@param bottom : [in] Bottom clipping plane value
		 *@param top : [in] Top clipping plane value
		 */
		template <typename TypeA, typename TypeB>
		static void						ortho			( SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB top, TypeB bottom);
		/**
		 * Builds a matrix that sets a non centered orthogonal projection from the given parameters
		 * The result is put in the matrix in parameter
		 *@param p_matrix : [out] The matrix that will receive the perspective matrix
		 *@param left : [in] Left clipping plane value
		 *@param right : [in] Right clipping plane value
		 *@param bottom : [in] Bottom clipping plane value
		 *@param top : [in] Top clipping plane value
		 *@param zNear : [in] Near clipping plane value
		 *@param zFar : [in] Far clipping plane value
		 */
		template <typename TypeA, typename TypeB>
		static void						ortho			( SquareMatrix<TypeA, 4> & p_matrix, TypeB left, TypeB right, TypeB bottom, TypeB top, TypeB zNear, TypeB zFar);
		/**
		 * Transforms a vector through a transformation matrix
		 *@param p_matrix : [in] The transformation matrix
		 *@param p_vertex : [in] The vector to transform
		 *@return The transformed vector
		 */
		template <typename TypeA, typename TypeB, size_t Count>
		static Point<TypeB, Count>		mult( SquareMatrix<TypeA, 4> const & p_matrix, Point<TypeB, Count> const & p_vertex);
		/**
		 * Multiplies 2 matrix
		 *@param p_matrixA : [in] The first matrix
		 *@param p_matrixB : [in] The second matrix
		 *@return The result matrix
		 */
		template <typename TypeA, typename TypeB>
		static SquareMatrix<TypeA, 4>	mult( SquareMatrix<TypeA, 4> const & p_matrixA, SquareMatrix<TypeB, 4> const & p_matrixB);
		/**
		 * switches the handness of a matrix (from right to left or left to right)
		 *@param p_matrix : [in] The matrix to switch
		 *@return The switched matrix
		 */
		template <typename TypeA>
		static SquareMatrix<TypeA, 4>	switch_hand	( SquareMatrix<TypeA, 4> const & p_matrix);
	};
}
}

#include "TransformationMatrix.inl"

#endif
