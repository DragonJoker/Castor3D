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
#ifndef ___C3D_Pave___
#define ___C3D_Pave___

#include "Mesh.hpp"

namespace Castor3D
{
	//! The cube representation
	/*!
	The calling of cube is abusive because it has parametrable dimensions (width, height and depth)
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Cube : public MeshCategory 
	{
	private:
		real m_height;	//!< The cube height
		real m_width;	//!< The cube width
		real m_depth;	//!< The cube depth

	public:
		/**
		 * Constructor
		 *@param p_width : [in] The wanted cube width
		 *@param p_height : [in] The wanted cube height
		 *@param p_depth : [in] The wanted cube depth
		 *@param p_name : [in] The mesh name
		 */
		Cube();
		/**
		 * Destructor
		 */
		~Cube();
		/**
		 * Generates the cube points
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
		inline real GetHeight	()const {return m_height;}
		inline real GetWidth	()const {return m_width;}
		inline real GetDepth	()const {return m_depth;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const Cube & c) { return o << "Pave(" << c.m_height << "," << c.m_width << "," << c.m_depth << ")";}
	};
}

#endif
