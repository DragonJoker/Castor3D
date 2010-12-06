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
#ifndef ___Castor_Container___
#define ___Castor_Container___

namespace Castor
{	namespace Math
{
	/*!
	A container will be a simple object which surrounds a geometry, mesh or/and submesh
	It can be a box, a sphere or other.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Container
	{
	protected:
		typedef Templates::Value<real> value;
		Point3r m_center; //!< The center of the container

	public:
		/**
		 * Constructor
		 */
		Container( const Point3r & p_center)
			:	m_center( p_center)
		{
		}
		/**
		 * Tests if a vertex is within the container, id est inside it but not on it's limits
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the container, false if not
		 */
		virtual bool IsWithin( const Point3r & p_v)=0;
		/**
		 * Tests if a vertex is on the limits of this container, and not within
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the container, false if not
		 */
		virtual bool IsOnLimits( const Point3r & p_v)=0;

	public:
		inline Point3r	GetCenter	()const { return m_center; }
	};

	/*!
	A combo box is a box container
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ComboBox : public Container, public MemoryTraced<ComboBox>
	{
	protected:
		Point3r m_min;	//!< The min extent of the combo box
		Point3r m_max; //!< The max extent of the combo box

	public:
		/**
		 * Constructor
		 */
		ComboBox( const Point3r & p_min=Point3r(), const Point3r & p_max=Point3r())
			:	Container( p_min + (p_max - p_min) / real( 2.0)),
				m_min( p_min),
				m_max( p_max)
		{}
		/**
		 * Tests if a vertex is within the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the box, false if not
		 */
		virtual bool IsWithin( const Point3r & p_v)
		{ 
			return (p_v[0] > m_min[0] && p_v[0] < m_max[0]) 
					&& (p_v[1] > m_min[1] && p_v[1] < m_max[1])
					&& (p_v[2] > m_min[2] && p_v[2] < m_max[2]);
		}
		/**
		 * Tests if a vertex is on the limits of the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the box, false if not
		 */
		virtual bool IsOnLimits( const Point3r & p_v)
		{
			return ! IsWithin( p_v) 
					&& (value::equals( p_v[0], m_min[0])
						|| value::equals( p_v[0], m_max[0])
						|| value::equals( p_v[1], m_min[1])
						|| value::equals( p_v[1], m_max[1])
						|| value::equals( p_v[2], m_min[2])
						|| value::equals( p_v[2], m_max[2]));
		}
		/**
		 * Reinitialises th combo box to the given limits
		 */
		void Load( const Point3r & p_ptMin, const Point3r & p_ptMax)
		{
			m_min = p_ptMin;
			m_max = p_ptMax;
		}

	public:
		inline const Point3r &	GetMin	()const	{ return m_min; }
		inline const Point3r &	GetMax	()const	{ return m_max; }
		inline Point3r &		GetMin	()		{ return m_min; }
		inline Point3r &		GetMax	()		{ return m_max; }

		inline void	SetMin	( const Point3r & p_ptMin) { m_min = p_ptMin; }
		inline void	SetMax	( const Point3r & p_ptMax) { m_max = p_ptMax; }
	};

	/*!
	A Sphere is a spheric container
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Sphere : public Container, public MemoryTraced<Sphere>
	{
	private:
		real m_radius;			//!< The radius of the sphere

	public:
		/**
		 * Constructor
		 */
		Sphere( const Point3r & p_center=Point3r(), real p_radius=0)
			:	Container( p_center),
				m_radius( p_radius)
		{
		}
		/**
		 * Constructor
		 */
		Sphere( const ComboBox & p_box)
			:	Container( p_box.GetCenter()),
				m_radius( (p_box.GetMax() - m_center).GetLength())
		{}

		void Load( const Point3r & p_center, real p_radius)
		{
			m_center = p_center;
			m_radius = p_radius;
		}
		void Load( const ComboBox & p_box)
		{
			m_center = p_box.GetCenter();
			m_radius = (p_box.GetMax() - m_center).GetLength();
		}
		/**
		 * Tests if a vertex is within the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the box, false if not
		 */
		virtual bool IsWithin( const Point3r & p_v) { return (p_v - m_center).GetLength() < m_radius; }
		/**
		 * Tests if a vertex is on the limits of the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the box, false if not
		 */
		virtual bool IsOnLimits( const Point3r & p_v) { return value::equals( (p_v - m_center).GetLength(), m_radius); }

	public:
		inline real	GetRadius	()const { return m_radius; }
	};
}
}

#endif