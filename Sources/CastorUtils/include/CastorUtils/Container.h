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
#ifndef ___C3D_ComboBox___
#define ___C3D_ComboBox___

namespace General
{	namespace Math
{
	/*!
	A container will be a simple object which is around a geometry, mesh or/and submesh
	It can be a box, a sphere or other.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Container
	{
	protected:
		Vector3f m_center; //!< The center of the container

	public:
		/**
		 * Constructor
		 */
		Container( const Vector3f & p_center)
		{
			m_center = Vector3f( p_center);
		}
		/**
		 * Tests if a vertex is within the container
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the container, false if not
		 */
		virtual bool IsWithin( const Vector3f & p_v)=0;
		/**
		 * Tests if a vertex is on the limits of this container
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the container, false if not
		 */
		virtual bool IsOnLimits( const Vector3f & p_v)=0;

	public:
		inline Vector3f	GetCenter	()const { return m_center; }
	};

	/*!
	A combo box is a box container
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class ComboBox : public Container
	{
	protected:
		Vector3f m_min;	//!< The min extent of the combo box
		Vector3f m_max; //!< The max extent of the combo box

	public:
		/**
		 * Constructor
		 */
		ComboBox( const Vector3f & p_min, const Vector3f & p_max)
			:	Container( p_min + (p_max - p_min) / 2.0f),
				m_min( p_min),
				m_max( p_max)
		{}
		/**
		 * Tests if a vertex is within the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is within the box, false if not
		 */
		virtual bool IsWithin( const Vector3f & p_v) { return (p_v.x >= m_min.x && p_v.x <= m_max.x) 
															&& (p_v.y >= m_min.y && p_v.y <= m_max.y)
															&& (p_v.z >= m_min.z && p_v.z <= m_max.z); }
		/**
		 * Tests if a vertex is on the limits of the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the box, false if not
		 */
		virtual bool IsOnLimits( const Vector3f & p_v) { return IsWithin( p_v) 
															&& (abs( p_v.x - m_min.x) < 0.0000001f
																|| abs( p_v.x - m_max.x) < 0.0000001f
																|| abs( p_v.y - m_min.y) < 0.0000001f
																|| abs( p_v.y - m_max.y) < 0.0000001f
																|| abs( p_v.z - m_min.z) < 0.0000001f
																|| abs( p_v.z - m_max.z) < 0.0000001f); }

	public:
		inline Vector3f	GetMin	()const { return m_min; }
		inline Vector3f	GetMax	()const { return m_max; }
	};

	/*!
	A Sphere is a spheric container
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Sphere : public Container
	{
	private:
		float m_radius;			//!< The radius of the sphere

	public:
		/**
		 * Constructor
		 */
		Sphere( const Vector3f & p_center, float p_radius)
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

		void Load( const Vector3f & p_center, float p_radius)
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
		virtual bool IsWithin( const Vector3f & p_v) { return (p_v - m_center).GetLength() <= m_radius; }
		/**
		 * Tests if a vertex is on the limits of the box
		 *@param p_v : [in] The vertex to test
		 *@return true if the vertex is on the limits of the box, false if not
		 */
		virtual bool IsOnLimits( const Vector3f & p_v) { return abs( (p_v - m_center).GetLength() - m_radius) < 0.0000001f; }

	public:
		inline float	GetRadius	()const { return m_radius; }
	};
}
}

#endif