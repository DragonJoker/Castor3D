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
#ifndef ___Castor_Quaternion___
#define ___Castor_Quaternion___

#include "Point.hpp"
#include "Loader.hpp"

namespace Castor
{	namespace Math
{
	class Quaternion;
}
}

namespace Castor
{	namespace Resources
{
	//! Quaternion loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Math::Quaternion>
	{
	public:
		/**
		 * Loads a resource from a binary file
		 *@param p_object : [in/out] The object to load
		 *@param p_file : [in/out] The file where to load the object
		 */
		static bool Load( Math::Quaternion & p_object, Utils::File & p_file);
		/**
		 * Saves a resource to a binary file
		 *@param p_object : [in] The object to save
		 *@param p_file : [in/out] The file where to save the object
		 */
		static bool Save( Math::Quaternion const & p_object, Utils::File & p_file);	
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Math::Quaternion & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( Math::Quaternion const & p_object, Utils::File & p_file);
	};
}
}

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
		Quaternion( Quaternion const & p_q);
		/**
		 * Copy constructor
		 */
		Quaternion( Point4r const & p_q);
		/**
		 * Constructor from rotation matrix
		 */
		Quaternion( Matrix4x4r const & p_matrix)
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
		Quaternion( Point3r const & p_vector, Angle const & p_angle)
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
		Quaternion &	operator = ( Quaternion const & p_q);
		Quaternion &	operator +=( Quaternion const & p_q);
		Quaternion &	operator -=( Quaternion const & p_q);
		Quaternion &	operator *=( Quaternion const & p_q);
		Quaternion &	operator *=( real p_fScalar);
		real			Dot		   ( Quaternion const & p_quat)const;
		void			Transform  ( Point3r const & p_vector, Point3r & p_ptResult);

	public:
		void ToRotationMatrix( Matrix4x4r & p_matrix)const;
		void ToRotationMatrix( real * p_matrix)const;
		void FromRotationMatrix( Matrix4x4r const & p_matrix);
		void FromRotationMatrix( real * p_matrix);
		void FromAxisAngle( Point3r const & p_vector, Angle const & p_angle);
		void ToAxisAngle( Point3r & p_vector, Angle & p_angle)const;

		Angle GetYaw()const;
		Angle GetPitch()const;
		Angle GetRoll()const;

		void Conjugate();
		QuaternionPtr GetConjugate();
		real GetMagnitude();
		void Normalise();
		Quaternion Slerp( Quaternion const & p_target, real p_percent, bool p_shortestPath);

		virtual bool Save( Utils::File & p_file)const;
		virtual bool Load( Utils::File & p_file);
	};

	Quaternion	operator +( Quaternion const & p_qA, Quaternion const & p_qB);
	Quaternion	operator -( Quaternion const & p_qA, Quaternion const & p_qB);
	Quaternion	operator *( Quaternion const & p_qA, Quaternion const & p_qB);
	Quaternion	operator *( Quaternion const & p_q, real p_fScalar);
	Quaternion	operator *( real p_scalar, Quaternion const & p_quat);
	Quaternion	operator -( Quaternion const & p_q);
}
}

#endif
