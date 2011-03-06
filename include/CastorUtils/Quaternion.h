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
#ifndef ___Castor_Quaternion___
#define ___Castor_Quaternion___

#include "Point.h"

namespace Castor
{	namespace Math
{
	//! Quaternion representation class
	/*!
	A quaternion is an axis and an angle, it's one of the best ways to represent orientations and rotations
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class Quaternion : public Point4r
	{
	public:
		static const Quaternion Identity;	//!< Identity quaternion
		static const Quaternion Null;		//!< Null quaternion

	public:
		/**
		 * Constructor
		 */
		Quaternion();
		/**
		 * Copy constructor
		 */
		Quaternion( const Quaternion & p_q);
		/**
		 * Copy constructor
		 */
		Quaternion( const Point4r & p_q);
		/**
		 * Constructor from rotation matrix
		 */
		Quaternion( const Matrix4x4r & p_matrix)
		{
			FromRotationMatrix( p_matrix);
		}
		/**
		 * Constructor from matrix
		 */
		Quaternion( real * p_matrix)
		{
			FromRotationMatrix( p_matrix);
		}
		/**
		 * Constructor from axis and angle
		 */
		Quaternion( const Point3r & p_vector, const Angle & p_angle)
		{
			FromAxisAngle( p_vector, p_angle);
		}
		/**
		 * Specified constructor
		 */
		Quaternion( real p_w, real p_x, real p_y, real p_z)
			:	Point4r( p_x, p_y, p_z, p_w)
		{
			Normalise();
		}
		/**
		 * Destructor
		 */
		virtual ~Quaternion();

	public:
		Quaternion & operator =( const Quaternion & p_q);
        Quaternion operator +( const Quaternion & q) const;
		Quaternion operator -( const Quaternion & q) const;
		Quaternion operator *( real p_fScalar) const;
		Quaternion operator *( const Quaternion & q)const;
		void operator *=( const Quaternion & q);
		Quaternion operator -() const;
		real Dot( const Quaternion & p_quat)const;
		Point3r operator *(const Point3r & p_vector)const;

	public:
		void ToRotationMatrix( Matrix4x4r & p_matrix)const;
		void ToRotationMatrix( real * p_matrix)const;
		void FromRotationMatrix( const Matrix4x4r & p_matrix);
		void FromRotationMatrix( real * p_matrix);
		void FromAxisAngle( const Point3r & p_vector, const Angle & p_angle);
		void ToAxisAngle( Point3r & p_vector, Angle & p_angle)const;

		Angle GetYaw()const;
		Angle GetPitch()const;
		Angle GetRoll()const;

		void Conjugate();
		QuaternionPtr GetConjugate();
		real GetMagnitude();
		void Normalise();
		Quaternion Slerp( const Quaternion & p_target, real p_percent, bool p_shortestPath);

		virtual bool Save( Utils::File & p_file)const;
		virtual bool Load( Utils::File & p_file);
	};
	/**
	 * Multiplies a quaternion by a scalar
	 *@param p_scalar : [in] The scalar by which the quaternion is multiplied
	 *@param p_quat : [in] The quaternion to multiply
	 *@return The multiplied quaternion
	 */
	Quaternion operator * ( real p_scalar, const Quaternion & p_quat);
}
}

#endif
