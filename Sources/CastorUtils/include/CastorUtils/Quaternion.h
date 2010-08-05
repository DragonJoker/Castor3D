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
#ifndef ___C3D_Quaternion___
#define ___C3D_Quaternion___

#include "RotationMatrix.h"
#include "File.h"
#include "Point.h"

namespace General
{	namespace Math
{
	/*!
	A quaternion is an axis and an angle, it's one of the best ways to represent orientations and rotations
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class Quaternion : public Point4D<float>
	{
	public:
		static const Quaternion Quat_Identity;	//!< Identity quaternion
		static const Quaternion Quat_Null;		//!< Null quaternion

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
		 * Constructor from rotation matrix
		 */
		Quaternion( RotationMatrix * p_matrix)
		{
			FromRotationMatrix( p_matrix);
		}
		/**
		 * Constructor from rotation matrix
		 */
		Quaternion( float * p_matrix)
		{
			FromRotationMatrix( p_matrix);
		}
		/**
		 * Constructor from axis and angle
		 */
		Quaternion( const Vector3f & p_vector, float p_degrees)
		{
			FromAxisAngle( p_vector, p_degrees);
		}
		/**
		 * Specified constructor
		 */
		Quaternion( float p_w, float p_x, float p_y, float p_z)
			:	Point4D<float>( p_x, p_y, p_z, p_w)
		{
			Normalise();
		}
		/**
		 * Destructor
		 */
		virtual ~Quaternion();

	public:
		void operator *=( const Quaternion & q);
		Quaternion & operator =( const Quaternion & p_q);
        Quaternion operator+ ( const Quaternion & q) const;
		Quaternion operator- ( const Quaternion & q) const;
		Quaternion operator* ( const Quaternion & q)const;
		Quaternion operator* ( float fScalar) const;
		Quaternion operator- () const;
		float Dot( const Quaternion & p_quat)const;
		Vector3f operator* (const Vector3f & p_vector)const;

	public:
		void ToRotationMatrix( RotationMatrix & p_matrix);
		void ToRotationMatrix( float * p_matrix)const;
		void FromRotationMatrix( RotationMatrix * p_matrix);
		void FromRotationMatrix( float * p_matrix);
		void FromAxisAngle( const Vector3f & p_vector, float p_degrees);
		void ToAxisAngle( Vector3f & p_vector, float & p_angle);

		float GetYaw();
		float GetPitch();
		float GetRoll();

		void Conjugate();
		Quaternion* GetConjugate();
		float GetMagnitude();
		void Normalise();
		Quaternion Slerp( const Quaternion & p_target, float p_percent, bool p_shortestPath);

		bool Write( Utils::File & p_file)const;
		bool Read( Utils::File & p_file);
	};
	/**
	 * Multiplies a quaternion by a scalar
	 *@param p_scalar : [in] The scalar by which the quaternion is multiplied
	 *@param p_quat : [in] The quaternion to multiply
	 *@return The multiplied quaternion
	 */
	Quaternion operator* ( float p_scalar, const Quaternion & p_quat);
}
}

#endif