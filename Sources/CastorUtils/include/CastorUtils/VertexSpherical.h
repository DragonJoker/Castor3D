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
#ifndef ___C3D_VertexSpherical___
#define ___C3D_VertexSpherical___

namespace General
{	namespace Math
{
	class VertexSpherical
	{
	public:
		float m_radius;		//!< The radius (distance to origin)
		float m_phi;		//!< The xy angle
		float m_theta;		//!< The xz angle

	public:
		/**
		* Copy constructor
		*/
		VertexSpherical( const VertexSpherical & p_vertex);
		/**
		 * Specified constructor
		 */
		VertexSpherical( float p_radius=0, float p_phi=0, float p_theta=0);
		/**
		 * Destructor
		 */
		~VertexSpherical();

	public:
	};
}
}

#endif