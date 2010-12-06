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
#ifndef ___C3D_Vertex___
#define ___C3D_Vertex___

#include "../Module_Geometry.h"

namespace Castor3D
{
	//! Representation of a vertex
	/*!
	The 3 coordinates of a vertex, it's normal, tangent and texture coordinates, an optional index for smoothing groups
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Vertex
	{
	public:
		static size_t Size;
		static unsigned long s_vertexNumber;	//!< Total number of created vertex
		size_t m_index;							//!< The index, for the smoothing groups

	private:
		Point3r m_ptCoords;
		Point3r m_ptNormal;
		Point3r m_ptTangent;
		Point2r m_ptTexCoord;
		real * m_pBuffer;
		bool m_bOwnBuffer;

	public:
		/**
		* Specified constructor
		*/
		Vertex( real x = 0, real y = 0, real z = 0, size_t p_index = 0);
		/**
		* Copy constructor
		*/
		Vertex( const Vertex & p_vertex);
		/**
		* Constructor from point
		*/
		Vertex( const Point3r & p_vertex);
		/**
		* Constructor from a real array
		*/
		Vertex( real * p_pBuffer);
		/**
		* Constructor from the difference between second and first argument
		*/
		Vertex( const Vertex & p_v1, const Vertex & p_v2);
		/**
		* Destructor
		*/
		~Vertex();

		void LinkCoords( real * p_pBuffer);
		void UnlinkCoords();

	public:
		inline Point3r &		GetCoords	()		{ return m_ptCoords; }
		inline Point3r &		GetNormal	()		{ return m_ptNormal; }
		inline Point3r &		GetTangent	()		{ return m_ptTangent; }
		inline Point2r &		GetTexCoord	()		{ return m_ptTexCoord; }
		inline real *			ptr			()		{ return m_pBuffer; }
		inline const Point3r &	GetCoords	()const	{ return m_ptCoords; }
		inline const Point3r &	GetNormal	()const	{ return m_ptNormal; }
		inline const Point3r &	GetTangent	()const	{ return m_ptTangent; }
		inline const Point2r &	GetTexCoord	()const	{ return m_ptTexCoord; }
		inline const real *		const_ptr	()const	{ return m_pBuffer; }

		inline void SetCoords	( const Point3r & val)		{ m_ptCoords = val; }
		inline void SetCoords	( real x, real y, real z)	{ m_ptCoords[0] = x;m_ptCoords[1] = y;m_ptCoords[2] = z; }
		inline void SetCoords	( const real * p_pCoords)	{ m_ptCoords[0] = p_pCoords[0];m_ptCoords[1] = p_pCoords[1];m_ptCoords[2] = p_pCoords[2]; }

		inline void SetNormal	( const Point3r & val)		{ m_ptNormal = val; }
		inline void SetNormal	( real x, real y, real z)	{ m_ptNormal[0] = x;m_ptNormal[1] = y;m_ptNormal[2] = z; }
		inline void SetNormal	( const real * p_pCoords)	{ m_ptNormal[0] = p_pCoords[0];m_ptNormal[1] = p_pCoords[1];m_ptNormal[2] = p_pCoords[2]; }

		inline void SetTangent	( const Point3r & val)		{ m_ptTangent = val; }
		inline void SetTangent	( real x, real y, real z)	{ m_ptTangent[0] = x;m_ptTangent[1] = y;m_ptTangent[2] = z; }
		inline void SetTangent	( const real * p_pCoords)	{ m_ptTangent[0] = p_pCoords[0];m_ptTangent[1] = p_pCoords[1];m_ptTangent[2] = p_pCoords[2]; }

		inline void SetTexCoord	( const Point2r & val)		{ m_ptTexCoord = val; }
		inline void SetTexCoord	( real x, real y)			{ m_ptTexCoord[0] = x;m_ptTexCoord[1] = y; }
		inline void SetTexCoord	( const real * p_pCoords)	{ m_ptTexCoord[0] = p_pCoords[0];m_ptTexCoord[1] = p_pCoords[1]; }

	private:
		void _link( const Point3r & p_ptCoords, const Point3r & p_ptNormal, const Point3r & p_ptTangent, const Point2r & p_ptTexCoords);
	};
}

#endif