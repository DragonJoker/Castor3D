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
#ifndef ___Interpolation___
#define ___Interpolation___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Class which makes the interpolations
	/*!
	Template class which handles the interpolations, would it be quaternion, vector, real, ... interpolations
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <class T>
	class C3D_API Interpolator
	{
	protected:
		T m_tSrc;		//!< The starting value
		T m_tCurrent;	//!< The current value
		T m_tDest;		//!< The ending value

	public:
		/**
		 * Constructor, which sets up the start and the end
		 *@param p_tSrc : [in] The start
		 *@param p_tDest : [in] The end
		 */
		Interpolator( const T & p_tSrc, const T & p_tDest)
			:	m_tSrc( p_tSrc),
				m_tCurrent( p_tSrc),
				m_tDest( p_tDest)
		{
		}
		/**
		 * Destructor, dummy
		 */
		~Interpolator()
		{
		}
		/**
		 * Interpolation function, virtual, must be implemented for the interpolator you create
		 *@param p_rPercent : [in] The percentage
		 */
		virtual T Interpolate( real p_rPercent)=0;
	};

	//! Class which makes the linear interpolations for vertex
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\brief Linear Vector interpolation class overload
	*/
	class C3D_API VertexLinearInterpolator : public Interpolator<Point3r>, public MemoryTraced<VertexLinearInterpolator>
	{
		/**
		 * Interpolation function, linear interpolation is easy...
		 *@param p_rPercent : [in] The percentage
		 */
		virtual Point3r Interpolate( real p_rPercent)
		{
			if (p_rPercent <= 0.0)
			{
				m_tCurrent = m_tSrc;
			}
			else if (p_rPercent >= 1.0)
			{
				m_tCurrent = m_tDest;
			}
			else
			{
				m_tCurrent = m_tSrc + ((m_tSrc - m_tDest) * p_rPercent);
			}

			return m_tCurrent;
		}
	};

	//! Class which makes the linear interpolations for quaternions
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\brief Linear Quaternion interpolation class overload
	*/
	class C3D_API QuaternionLinearInterpolator : public Interpolator<Quaternion>, public MemoryTraced<QuaternionLinearInterpolator>
	{
	public:
		/**
		 * Interpolation function, linear interpolation is easy...
		 *@param p_rPercent : [in] The percentage
		 */
		virtual Quaternion Interpolate( real p_rPercent)
		{
			m_tCurrent = m_tCurrent * (m_tDest - m_tSrc).Slerp( Quaternion::Identity, 1.0f - p_rPercent, true);
			return m_tCurrent;
		}
	};
}

#endif
