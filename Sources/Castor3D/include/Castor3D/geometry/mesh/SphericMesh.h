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
#ifndef ___C3D_Sphere___
#define ___C3D_Sphere___

#include "Mesh.h"

namespace Castor3D
{
	//! The sphere primitive
	/*!
	This sphere has squared faces. It is built from a radius and a number of subsections
	\author Sylvain DOREMUS
	\date 14/02/2010
	\todo Correct the vertex generation bug
	*/
	class C3D_API SphericMesh : public Mesh
	{
	private:
		friend class MeshManager;
		real m_radius;				//!< The sphere radius, the basic sphere has a radius of 1.0 then is scaled to have the good radius
		unsigned int m_nbFaces;		//!< The subsections number
		/**
		 * Constructor, only the MeshManager can use it
		 *@param p_radius : [in] The wanted radius
		 *@param p_nbFaces : [in] The wanted subsections number
		 *@param p_name : [in] The mesh name
		 */
		SphericMesh( real p_radius=0.0, unsigned int p_nbFaces=0,
					 const String & p_name = C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~SphericMesh();
		/**
		 * Generates the sphere points
		 */
		virtual void GeneratePoints();
		/**
		 * Modifies sphere caracteristics and rebuilds it
		 *@param p_radius : [in] The new wanted radius
		 *@param p_nbFaces : [in] The new wanted subsections number
		 */
		void Modify( real p_radius, unsigned int p_nbFaces);

	public:
		/**@name Accessors */
		//@{
		inline unsigned int	GetNbFaces	()const {return m_nbFaces;}
		inline real		GetRadius	()const {return m_radius;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const SphericMesh & c) { return o << "Sphere(" << c.m_nbFaces << "," << c.m_radius << ")";}
	};
}

#endif
