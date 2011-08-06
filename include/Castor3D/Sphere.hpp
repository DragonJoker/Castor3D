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
#ifndef ___C3D_Sphere___
#define ___C3D_Sphere___

#include "Mesh.hpp"

namespace Castor3D
{
	//! The sphere primitive
	/*!
	This sphere has squared faces. It is built from a radius and a number of subsections
	\author Sylvain DOREMUS
	\date 14/02/2010
	\todo Correct the vertex generation bug
	*/
	class C3D_API Sphere : public MeshCategory
	{
	private:
		real m_radius;				//!< The sphere radius, the basic sphere has a radius of 1.0 then is scaled to have the good radius
		unsigned int m_nbFaces;		//!< The subsections number

	public:
		/**
		 * Constructor
		 */
		Sphere();
		/**
		 * Destructor
		 */
		~Sphere();
		/**
		 * Generates the sphere points
		 */
		virtual void GeneratePoints();
		/**
		 * Clone function, used by Factory
		 */
		virtual MeshCategoryPtr Clone();
		/**
		 * Modifies the mesh caracteristics then rebuild it
		 *@param p_arrayFaces : [in] The new wanted mesh faces number (unused)
		 *@param p_arrayDimensions : [in] The new wanted mesh dimensions
		 */
		virtual void Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions);

	public:
		/**@name Accessors */
		//@{
		inline unsigned int	GetNbFaces	()const {return m_nbFaces;}
		inline real			GetRadius	()const {return m_radius;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const Sphere & c) { return o << "Sphere(" << c.m_nbFaces << "," << c.m_radius << ")";}
	};
}

#endif
