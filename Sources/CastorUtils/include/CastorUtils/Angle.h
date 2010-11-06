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
#ifndef ___CastorAngle___
#define ___CastorAngle___

#include "Module_Utils.h"

namespace Castor
{	namespace Math
{
	/*!
		Helper class to manage angles without taking care of degrees or radians
	*/
	class Angle
	{
	private:
		real m_rAngle; //!< Angle in radian

	public:
		static real RadiansToDegrees;
		static real DegreesToRadians;
		static real Pi;
		static real PiMult2;
		static real PiDiv2;

	public:
		Angle( real p_radians = 0)
			:	m_rAngle( p_radians)
		{
		}
		Angle( const Angle & p_angle)
			:	m_rAngle( p_angle.m_rAngle)
		{
		}
		~Angle()
		{
		}
		real Degrees()const
		{
			return m_rAngle * RadiansToDegrees;
		}
		real Radians()const
		{
			return m_rAngle;
		}
		void Degrees( real p_rAngle)
		{
			m_rAngle = p_rAngle * DegreesToRadians;
		}
		void Radians( real p_rAngle)
		{
			m_rAngle = p_rAngle;
		}
		/**
		 *		+ operators
		 */
		Angle operator +( const Angle & p_angle)const
		{
			Angle l_return( *this);
			l_return.m_rAngle += p_angle.m_rAngle;
		}
		void operator +=( const Angle & p_angle)
		{
			m_rAngle += p_angle.m_rAngle;
		}
		Angle operator +( real p_radians)const
		{
			Angle l_return( *this);
			l_return.m_rAngle += p_radians;
		}
		void operator +=( real p_radians)
		{
			m_rAngle += p_radians;
		}
		/**
		 *		- operators
		 */
		Angle operator -( const Angle & p_angle)const
		{
			Angle l_return( *this);
			l_return.m_rAngle -= p_angle.m_rAngle;
		}
		void operator -=( const Angle & p_angle)
		{
			m_rAngle -= p_angle.m_rAngle;
		}
		Angle operator -( real p_radians)const
		{
			Angle l_return( *this);
			l_return.m_rAngle -= p_radians;
		}
		void operator -=( real p_radians)
		{
			m_rAngle -= p_radians;
		}
		/**
		 *		* operators
		 */
		Angle operator *( const Angle & p_angle)const
		{
			Angle l_return( *this);
			l_return.m_rAngle *= p_angle.m_rAngle;
		}
		void operator *=( const Angle & p_angle)
		{
			m_rAngle *= p_angle.m_rAngle;
		}
		Angle operator *( real p_scalar)const
		{
			Angle l_return( *this);
			l_return.m_rAngle *= p_scalar;
		}
		void operator *=( real p_scalar)
		{
			m_rAngle *= p_scalar;
		}
		/**
		 *		/ operators
		 */
		Angle operator /( const Angle & p_angle)const
		{
			Angle l_return( *this);
			l_return.m_rAngle /= p_angle.m_rAngle;
		}
		void operator /=( const Angle & p_angle)
		{
			m_rAngle /= p_angle.m_rAngle;
		}
		Angle operator /( real p_scalar)const
		{
			Angle l_return( *this);
			l_return.m_rAngle /= p_scalar;
		}
		void operator /=( real p_scalar)
		{
			m_rAngle /= p_scalar;
		}
		/**
		 *		= operators
		 */
		Angle & operator =( const Angle & p_angle)
		{
			m_rAngle = p_angle.m_rAngle;
			return * this;
		}
		Angle & operator =( real p_radians)
		{
			m_rAngle = p_radians;
			return * this;
		}
	};
}
}

#endif