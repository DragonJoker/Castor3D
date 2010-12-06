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
#ifndef ___C3D_Cylinder___
#define ___C3D_Cylinder___

#include "Mesh.h"

namespace Castor3D
{
	//! The cylinder representation
	/*!
	The cylinder is a basic primitive, with a parametrable number of faces
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API CylindricMesh : public Mesh
	{
	private:
		friend class MeshManager;
		real m_height;				//!< The cylinder height
		real m_radius;				//!< The cylinder radius
		unsigned int m_nbFaces;		//!< The number of subdivisions
		/**
		 * Constructor, only MeshManager can use it
		 *@param p_height : [in] The wanted cylinder height
		 *@param p_radius : [in] The wanted cylinder radius
		 *@param p_nbFaces : [in] The wanted cylinder subdivisions number
		 *@param p_name : [in] The mesh name
		 */
		CylindricMesh( real p_height=0.0, real p_radius=0.0, unsigned int p_nbFaces=1,
					   const String & p_name = C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~CylindricMesh();
		/**
		 * Generates the cylinder points
		 */
		virtual void GeneratePoints();
		/**
		 * Modifies the cylinder caracteristics then rebuild it
		 *@param p_height : [in] The new wanted cylinder height
		 *@param p_radius : [in] The new wanted cylinder radius
		 *@param p_nbFaces : [in] The new wanted cylinder subdivisions number
		 */
		void Modify( real p_radius, real p_height, unsigned int p_nbFaces);

	public:
		/**@name Accessors */
		//@{
		inline unsigned int	GetNbFaces	()const {return m_nbFaces;}
		inline real		GetHeight	()const {return m_height;}
		inline real		GetRadius	()const {return m_radius;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const CylindricMesh & c) { return o << "Cylinder(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";}
	};
}

#endif
