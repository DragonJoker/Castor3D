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
#ifndef ___C3D_PlaneMesh___
#define ___C3D_PlaneMesh___

#include "Mesh.h"

namespace Castor3D
{
	//! The plane representation
	/*!
	A plane can be subdivided in width and in depth.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API PlaneMesh : public Mesh
	{
	private:
		friend class MeshManager;
		real m_depth;					//!< The plane depth
		real m_width;					//!< The plane width
		unsigned int m_subDivisionsW;	//!< The number of depth subsections
		unsigned int m_subDivisionsD;	//!< The number of width subsections
		/**
		 * Constructor, only MeshManager can use it
		 *@param p_width : [in] The wanted width
		 *@param p_depth : [in] The wanted depth
		 *@param p_subdivisionsWidth : [in] The wanted width subsections
		 *@param p_subdivisionsDepth : [in] The wanted depth subsections
		 *@param p_name : [in] The mesh name
		 */
		PlaneMesh( real p_width=0.0, real p_depth=0.0,
				   unsigned int p_subdivisionsDepth=1, unsigned int p_subdivisionsWidth=1,
				   const String & p_name = C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~PlaneMesh();
		/**
		 * Generates the plane points
		 */
		virtual void GeneratePoints();
		/**
		 * Modifies the plane caracteristics and rebuilds it
		 *@param p_width : [in] The new wanted width
		 *@param p_depth : [in] The new wanted depth
		 *@param p_subDivX : [in] The new wanted width subsections
		 *@param p_subDivY : [in] The new wanted depth subsections
		 */
		void Modify( real p_width, real p_depth,
					 unsigned int p_subDivX, unsigned int p_subDivY);

	public:
		/**@name Accessors */
		//@{
		inline real		GetLength			()const {return m_depth;}
		inline real		GetWidth			()const {return m_width;}
		inline unsigned int	GetSubDivisionsX	()const {return m_subDivisionsW;}
		inline unsigned int	GetSubDivisionsY	()const {return m_subDivisionsD;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const PlaneMesh & c) { return o << "Plane(" << c.m_depth << "," << c.m_width << "," << c.m_subDivisionsW << "," << c.m_subDivisionsD << ")";}
	};
}

#endif
