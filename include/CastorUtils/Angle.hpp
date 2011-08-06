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
#ifndef ___CastorAngle___
#define ___CastorAngle___

#include "CastorUtils.hpp"
#include "Serialisable.hpp"

namespace Castor
{	namespace Math
{
	//! Angle helper class
	/*!
	Used to manage angles without taking care of degrees or radians
	*/
	class Angle : public Utils::Serialisable, public MemoryTraced<Angle>
	{
	private:
		real m_rAngle; //!< Angle in radian

	public:
		static real RadianToDegree;
		static real RadianToGrad;
		static real DegreeToRadian;
		static real DegreeToGrad;
		static real GradToDegree;
		static real GradToRadian;
		static real Pi;
		static double PiDouble;
		static float PiFloat;
		static real PiMult2;
		static real PiDiv2;

	private:
		// private so specified construction only from named ctors
		Angle( real p_radians);

	public:
		static Angle FromDegrees	( real p_degrees);
		static Angle FromRadians	( real p_radians);
		static Angle FromGrads		( real p_grads);

		Angle();
		Angle( Angle const & p_angle);
		~Angle();

		/**@name Conversion */
		//@{
		real	Degrees	()const;
		real	Radians	()const;
		real	Grads	()const;
		Angle &	Degrees	( real p_rAngle);
		Angle &	Radians	( real p_rAngle);
		Angle &	Grads	( real p_rAngle);
		//@}

		/**@name Trigonometric Functions */
		//@{
		real	Cos		()const;
		real	Sin		()const;
		real	Tan		()const;
		real	Cosh	()const;
		real	Sinh	()const;
		real	Tanh	()const;
		Angle &	ACos	( real p_rValue);
		Angle &	ASin	( real p_rValue);
		Angle &	ATan	( real p_rValue);
		//@}

		/**@name Operators */
		//@{
		Angle & operator = ( Angle const & p_angle);
		Angle & operator +=( Angle const & p_angle);
		Angle & operator -=( Angle const & p_angle);
		Angle & operator *=( Angle const & p_angle);
		Angle & operator /=( Angle const & p_angle);
		//@}

		DECLARE_SERIALISE_MAP()
	};

	Angle operator +( Angle const & p_angleA, Angle const & p_angleB);
	Angle operator -( Angle const & p_angleA, Angle const & p_angleB);
	Angle operator *( Angle const & p_angleA, Angle const & p_angleB);
	Angle operator /( Angle const & p_angleA, Angle const & p_angleB);
}
}

#endif