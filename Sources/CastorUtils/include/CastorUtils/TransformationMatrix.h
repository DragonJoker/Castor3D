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

namespace Castor
{	namespace Math
{
	/*!
	Holds the rotation of a 4x4 transformation matrix
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class TransformationMatrix : public Matrix4x4r
	{
	public:
		TransformationMatrix();
		TransformationMatrix( const TransformationMatrix & p_matrix);
		TransformationMatrix( const Angle & p_angle, const Point3r & p_axis);
		TransformationMatrix( const Quaternion & p_quat);
		virtual ~TransformationMatrix();

	public:
		void SetRotation( const Angle & p_angle, const Point3r & p_axis);
		void SetRotation( const Quaternion & p_quat);
		void SetRotationEuler( const Angle & p_rx, const Angle & p_ry, const Angle & p_rz);
		void SetScale( real p_hx, real p_hy, real p_hz);
		void SetScale( const Point3r & p_scale);
		void SetTranslation( real p_tx, real p_ty, real p_tz);
		void SetTranslation( const Point3r & p_translation);

		void Rotate( const Angle & p_angle, const Point3r & p_axis);
		void Rotate( const Quaternion & p_quat);
		/**
		 * Y axis rotation
		 */
		void Yaw( const Angle & p_angle);
		/**
		 * X axis rotation
		 */
		void Pitch( const Angle & p_angle);
		/**
		 * Z axis rotation
		 */
		void Roll( const Angle & p_angle);
		void Scale( real p_hx, real p_hy, real p_hz);
		void Scale( const Point3r & p_scale);
		void Translate( real p_tx, real p_ty, real p_tz);
		void Translate( const Point3r & p_translation);

		TransformationMatrix operator *( const TransformationMatrix & p_matrix);
		void operator *=( const TransformationMatrix & p_matrix);
		Point3r operator *( Point3rPtr p_vertex);
		Point3r operator *( const Point3r & p_vertex);
	};
}
}

#endif