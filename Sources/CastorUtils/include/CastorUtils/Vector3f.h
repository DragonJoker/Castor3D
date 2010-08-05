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
#ifndef ___C3D_Vector3f___
#define ___C3D_Vector3f___

#include "Point.h"

namespace General
{	namespace Math
{
	/*!
	Just 3 coordinates, an optional index for smoothing groups
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class Vector3f : public Point3D<float>
	{
	public:
		static unsigned long s_vertexNumber;	//!< Total number of vertex
		static Vector3f ZeroVertex;

	public :
		size_t m_index;							//!< The index, for the smoothing groups

	public :
		/**
		* Specified constructor
		*/
		Vector3f( float x = 0.0, float y = 0.0, float z = 0.0, size_t p_index = 0)
			:	Point3D<float>( x, y, z),
				m_index	( p_index)
		{
			s_vertexNumber++;
		}
		/**
		* Copy constructor
		*/
		Vector3f( const Vector3f & p_vertex)
			:	Point3D<float>( p_vertex)
		{
			s_vertexNumber++;
		}
		/**
		* Constructor from point
		*/
		Vector3f( const Point3D<float> & p_vertex)
			:	Point3D<float>( p_vertex)
		{
			s_vertexNumber++;
		}
		/**
		* Constructor from a float array
		*/
		Vector3f( const float * p_coord)
			:	Point3D<float>( p_coord)
		{
			s_vertexNumber++;
		}
		/**
		* Constructor from the difference between second and first argument
		*/
		Vector3f( const Vector3f & p_v1, const Vector3f & p_v2)
			:	Point3D<float>( p_v2.x - p_v1.x, p_v2.y - p_v1.y, p_v2.z - p_v1.z)
		{
			s_vertexNumber++;
		}
		/**
		* Destructor
		*/
		~Vector3f()
		{
			s_vertexNumber--;
		}

	public:
		inline void TranslateVertex( Vector3f & p_vertex)const
		{
			p_vertex.x += x;
			p_vertex.y += y;
			p_vertex.z += z;
		}
		inline void TranslateVertexXY( Vector3f & p_vertex)const
		{
			p_vertex.x += x;
			p_vertex.y += y;
		}
		inline void TranslateVertexXZ( Vector3f & p_vertex)const
		{
			p_vertex.x += x;
			p_vertex.z += z;
		}
		inline void TranslateVertexYZ( Vector3f & p_vertex)const
		{
			p_vertex.y += y;
			p_vertex.z += z;
		}

	public:
		inline Vector3f	operator +( const Vector3f & p_vertex)const
		{
			return Vector3f( x + p_vertex.x, y + p_vertex.y, z + p_vertex.z);
		}
		inline Vector3f operator -( const Vector3f & p_vertex)const
		{
			return Vector3f( x - p_vertex.x, y - p_vertex.y, z - p_vertex.z);
		}
		inline Vector3f operator *( const Vector3f & p_vertex)const
		{
			return Vector3f( x * p_vertex.x, y * p_vertex.y, z * p_vertex.z);
		}
		inline Vector3f operator *( int p_value)const
		{
			return Vector3f( x * p_value, y * p_value, z * p_value);
		}
		inline Vector3f operator *( float p_value)const
		{
			return Vector3f( x * p_value, y * p_value, z * p_value);
		}
		inline bool operator !=( const Vector3f & p_vertex)const
		{
			return (x != p_vertex.x) || (y != p_vertex.y) || (z != p_vertex.z);
		}
		inline bool operator ==( const Vector3f & p_vertex)const
		{
			return (abs( x - p_vertex.x) < 0.0001f) && (abs( y - p_vertex.y) < 0.0001f) && (abs( z - p_vertex.z) < 0.0001f);
		}
	};
}
}

#endif