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

#include "Module_Utils.h"
#include "Matrix.h"

namespace General
{	namespace Math
{
	/*!
	Holds the rotation of a 4x4 transformation matrix
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class RotationMatrix : public Matrix4<float>
	{
	public:
		RotationMatrix();
		virtual ~RotationMatrix();

	public:
		void Initialise();
		void LoadIdentity();
		void LoadRotationX( float p_angle);
		void LoadRotationY( float p_angle);
		void LoadRotationZ( float p_angle);
		void LoadRotation( float p_angle, const Vector3f & p_axis);
		void LoadRotationEuler( float p_rx, float p_ry, float p_rz);
		void LoadScale( float p_hx, float p_hy, float p_hz);
		void LoadTranslation( float p_tx, float p_ty, float p_tz);
		RotationMatrix * GetTranspose();

		void Multiply( Vector3f * p_vertex);
		float GetTrace();
	};
}
}

#endif