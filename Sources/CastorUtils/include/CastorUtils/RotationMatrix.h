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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_RotationMatrix___
#define ___C3D_RotationMatrix___

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
	class RotationMatrix : public Matrix<real, 4, 4>
	{
	public:
		RotationMatrix();
		virtual ~RotationMatrix();

	public:
		void LoadRotationX( real p_angle);
		void LoadRotationY( real p_angle);
		void LoadRotationZ( real p_angle);
		void LoadRotation( real p_angle, const Point3r & p_axis);
		void LoadRotation( const Quaternion & p_quat);
		void LoadRotationEuler( real p_rx, real p_ry, real p_rz);
		void LoadScale( real p_hx, real p_hy, real p_hz);
		void LoadTranslation( real p_tx, real p_ty, real p_tz);
		RotationMatrixPtr GetTranspose();

		void Multiply( Point3rPtr p_vertex);
		real GetTrace();
	};
}
}

#endif