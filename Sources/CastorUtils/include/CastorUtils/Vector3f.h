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
#ifndef ___Castor_Vertex___
#define ___Castor_Vertex___

#include "Point.h"

namespace Castor
{	namespace Math
{
	/*!
	Just 3 coordinates, an optional index for smoothing groups
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class Vertex : public Point3r
	{
	private:
		typedef Templates::Value<real> value;
	public:
		static unsigned long s_vertexNumber;	//!< Total number of vertex
		static Vertex ZeroVertex;

	public :
		size_t m_index;							//!< The index, for the smoothing groups

	public :
		/**
		* Specified constructor
		*/
		Vertex( real x = 0, real y = 0, real z = 0, size_t p_index = 0)
			:	Point3r( x, y, z),
				m_index	( p_index)
		{
			s_vertexNumber++;
		}
		/**
		* Copy constructor
		*/
		Vertex( const Vertex & p_vertex)
			:	Point3r( p_vertex)
		{
			s_vertexNumber++;
		}
		/**
		* Constructor from point
		*/
		Vertex( const Point3r & p_vertex)
			:	Point3r( p_vertex)
		{
			s_vertexNumber++;
		}
		/**
		* Constructor from a real array
		*/
		Vertex( const real * p_coord)
			:	Point3r( p_coord)
		{
			s_vertexNumber++;
		}
		/**
		* Constructor from the difference between second and first argument
		*/
		Vertex( const Vertex & p_v1, const Vertex & p_v2)
			:	Point3r( p_v2 - p_v1)
		{
			s_vertexNumber++;
		}
		/**
		* Destructor
		*/
		~Vertex()
		{
			s_vertexNumber--;
		}

	public:
		inline void TranslateVertex( Vertex & p_vertex)const
		{
			p_vertex += (* this);
		}
		inline void TranslateVertexXY( Vertex & p_vertex)const
		{
			p_vertex[0] += m_coords[0];
			p_vertex[1] += m_coords[1];
		}
		inline void TranslateVertexXZ( Vertex & p_vertex)const
		{
			p_vertex[0] += m_coords[0];
			p_vertex[2] += m_coords[2];
		}
		inline void TranslateVertexYZ( Vertex & p_vertex)const
		{
			p_vertex[1] += m_coords[1];
			p_vertex[2] += m_coords[2];
		}
	};
}
}

#endif