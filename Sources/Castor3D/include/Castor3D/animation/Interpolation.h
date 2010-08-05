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
#ifndef ___Interpolation___
#define ___Interpolation___

namespace Castor3D
{
	//! Class which makes the interpolations
	/*!
	Template class which handles the interpolations, would it be quaternion, vector, float, ... interpolations
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <class T>
	class CS3D_API Interpolator
	{
	protected:
		//! The starting value
		T m_src;
		//! The current value
		T m_current;
		//! The ending value
		T m_dest;

	public:
		/**
		 * Constructor, which sets up the start and the end
		 *@param p_src : the start
		 *@param p_dest : the end
		 */
		Interpolator( const T & p_src, const T & p_dest)
			:	m_src( p_src),
				m_current( p_src),
				m_dest( p_dest)
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
		 *@param p_percent : the percentage
		 */
		virtual T Interpolate( float p_percent)=0;
	};

	//! Class which makes the linear interpolations for vertex
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\brief Linear Vector interpolation class overload
	*/
	class CS3D_API VertexLinearInterpolator : public Interpolator<Vector3f>
	{
		/**
		 * Interpolation function, linear interpolation is easy...
		 *@param p_percent : the percentage
		 */
		virtual Vector3f Interpolate( float p_percent)
		{
			m_current = Vector3f( m_dest, m_src);
			if (p_percent <= 0.0 || p_percent > 1.0)
			{
				return m_dest;
			}
			m_current /= p_percent;
			return m_src + m_current;
		}
	};

	//! Class which makes the linear interpolations for quaternions
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\brief Linear Quaternion interpolation class overload
	*/
	class CS3D_API QuaternionLinearInterpolator : public Interpolator<Quaternion>
	{
	public:
		/**
		 * Interpolation function, linear interpolation is easy...
		 *@param p_percent : the percentage
		 */
		virtual Quaternion Interpolate( float p_percent)
		{
			m_current = m_current * (m_dest - m_src).Slerp( Quaternion::Quat_Identity, 1.0f - p_percent, true);
			return m_current;
		}
	};
}

#endif
