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
#ifndef ___C3D_Cone___
#define ___C3D_Cone___

#include "Mesh.h"

namespace Castor3D
{
	//! The cone representation
	/*!
	The cone is a basic primitive, with a parametrable number of faces
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Cone : public Mesh
	{
	private:
		friend class MeshManager;
		unsigned int m_nbFaces;		//!< The number of subdivisions
		real m_height;				//!< The cone height
		real m_radius;				//!< The cone radius
		/**
		 * Constructor, only MeshManager can use it
		 *@param p_height : [in] The wanted cone height
		 *@param p_radius : [in] The wanted cone radius
		 *@param p_nbFaces : [in] The wanted cone subdivisions number
		 *@param p_name : [in] The mesh name
		 */
		Cone( MeshManager * p_pManager, real p_height=0.0, real p_radius=0.0, unsigned int p_nbFaces=1,
				   const String & p_name = C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~Cone();
		/**
		 * Generates the cone points
		 */
		virtual void GeneratePoints();
		/**
		 * Modifies the cone caracteristics and rebuilds it
		 *@param p_radius : [in] The new wanted cone radius
		 *@param p_height : [in] The new wanted cone height
		 *@param p_nbFaces : [in] The new wanted cone subdivisions number
		 */
		void Modify( real p_radius, real p_height, unsigned int p_nbFaces);

	public:
		/**@name Accessors */
		//@{
		inline unsigned int	GetNbFaces	()const {return m_nbFaces;}
		inline real		GetHeight	()const {return m_height;}
		inline real		GetRadius	()const {return m_radius;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const Cone & c) { return o << "Cone(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";}
	};
}

#endif
