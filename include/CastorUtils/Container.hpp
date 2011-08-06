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
#ifndef ___Castor_Container___
#define ___Castor_Container___

namespace Castor
{	namespace Math
{
	//! Container class
	/*!
	A container will be a simple object which surrounds a geometry, mesh or/and submesh
	It can be a box, a sphere or other.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ContainerBox
	{
	protected:
		typedef Templates::Policy<real> policy;
		Point3r m_center; //!< The center of the container

	public:
		/**
		 * Copy Constructor
		 */
		ContainerBox( ContainerBox const & p_box)
		{
			m_center.copy( p_box.m_center);
		}
		/**
		 * Default Constructor
		 */
		ContainerBox()
		{
		}
		/**
		 * Constructor
		 */
		ContainerBox( Point3r const & p_center)
		{
			m_center.copy( p_center);
		}
		/**
		 * Assignment operator
		 */
		ContainerBox & operator =( ContainerBox const & p_box)
		{
			m_center.copy( p_box.m_center);
			return * this;
		}
		/**
		 * Tests if a vertex is within the container, id est inside it but not on it's limits
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the container, false if not
		 */
		virtual bool IsWithin( Point3r const & p_v)=0;
		/**
		 * Tests if a vertex is on the limits of this container, and not within
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the container, false if not
		 */
		virtual bool IsOnLimits( Point3r const & p_v)=0;

	public:
		/**@name Accessors */
		//@{
		inline Point3r const &	GetCenter	()const { return m_center; }
		inline Point3r &	    GetCenter	()      { return m_center; }
		//@}
	};

	//! Combo box container class
	/*!
	A combo box is a box container
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CubeBox : public ContainerBox, public MemoryTraced<CubeBox>
	{
	protected:
		Point3r m_min;	//!< The min extent of the combo box
		Point3r m_max; //!< The max extent of the combo box

	public:
		/**
		 * Default Constructor
		 */
		CubeBox()
			:	ContainerBox()
		{
		}
		/**
		 * Copy Constructor
		 */
		CubeBox( CubeBox const & p_box)
			:	ContainerBox( p_box)
		{
			m_min.copy( p_box.m_min);
			m_max.copy( p_box.m_max);
		}
		/**
		 * Constructor
		 */
		CubeBox( Point3r const & p_min, Point3r const & p_max)
			:	ContainerBox( p_min + (p_max - p_min) / real( 2.0))
		{
			m_min.copy( p_min);
			m_max.copy( p_max);
		}
		/**
		 * Assignment operator
		 */
		CubeBox & operator =( CubeBox const & p_box)
		{
			ContainerBox::operator =( p_box);
			m_min.copy( p_box.m_min);
			m_max.copy( p_box.m_max);
			return * this;
		}
		/**
		 * Tests if a vertex is within the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the box, false if not
		 */
		virtual bool IsWithin( Point3r const & p_v)
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
		virtual bool IsOnLimits( Point3r const & p_v)
		{
			return ! IsWithin( p_v)
					&& (policy::equals( p_v[0], m_min[0])
						|| policy::equals( p_v[0], m_max[0])
						|| policy::equals( p_v[1], m_min[1])
						|| policy::equals( p_v[1], m_max[1])
						|| policy::equals( p_v[2], m_min[2])
						|| policy::equals( p_v[2], m_max[2]));
		}
		/**
		 * Reinitialises th combo box to the given limits
		 */
		void Load( Point3r const & p_ptMin, Point3r const & p_ptMax)
		{
			m_min.copy( p_ptMin);
			m_max.copy( p_ptMax);
		}

	public:
		/**@name Accessors */
		//@{
		inline Point3r const &	GetMin	()const	{ return m_min; }
		inline Point3r const &	GetMax	()const	{ return m_max; }
		inline Point3r &		GetMin	()		{ return m_min; }
		inline Point3r &		GetMax	()		{ return m_max; }

		inline void	SetMin	( Point3r const & p_ptMin) { m_min.copy( p_ptMin); }
		inline void	SetMax	( Point3r const & p_ptMax) { m_max.copy( p_ptMax); }
		//@}
	};

	//! Sphere container class
	/*!
	A Sphere is a spheric container
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class SphereBox : public ContainerBox, public MemoryTraced<SphereBox>
	{
	private:
		real m_radius;			//!< The radius of the sphere

	public:
		/**
		 * Default Constructor
		 */
		SphereBox()
			:	ContainerBox(),
				m_radius( 0)
		{
		}
		/**
		 * Copy Constructor
		 */
		SphereBox( SphereBox const & p_box)
			:	ContainerBox( p_box)
			,	m_radius( p_box.m_radius)
		{
		}
		/**
		 * Constructor
		 */
		SphereBox( Point3r const & p_center, real p_radius)
			:	ContainerBox( p_center),
				m_radius( p_radius)
		{
		}
		/**
		 * Constructor from CubeBox
		 */
		SphereBox( CubeBox const & p_box)
			:	ContainerBox( p_box.GetCenter()),
				m_radius( real( (p_box.GetMax() - m_center).length()))
		{
		}
		/**
		 * Assignment operator
		 */
		SphereBox & operator =( SphereBox const & p_box)
		{
			ContainerBox::operator =( p_box);
			m_radius = p_box.m_radius;
			return * this;
		}

		void Load( Point3r const & p_center, real p_radius)
		{
			m_center.copy( p_center);
			m_radius = p_radius;
		}
		void Load( CubeBox const & p_box)
		{
			m_center.copy( p_box.GetCenter());
			m_radius = real( (p_box.GetMax() - m_center).length());
		}
		/**
		 * Tests if a vertex is within the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the box, false if not
		 */
		virtual bool IsWithin( Point3r const & p_v) { return (p_v - m_center).length() < m_radius; }
		/**
		 * Tests if a vertex is on the limits of the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the box, false if not
		 */
		virtual bool IsOnLimits( Point3r const & p_v) { return policy::equals( real( (p_v - m_center).length()), m_radius); }

	public:
		/**@name Accessors */
		//@{
		inline real	GetRadius	()const { return m_radius; }
		//@}
	};
}
}

#endif
