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
#ifndef ___C3D_Icosaedre___
#define ___C3D_Icosaedre___

#include "Mesh.h"

namespace Castor3D
{
	//! The icosaedron representation
	/*!
	An icosaedron is a geometric polygon constituted by 20 equilateral triangles. 
	This mesh uses is to build a sphere with triangular faces.
	It is subdivided to increase the precision of the sphere.
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API IcosaedricMesh : public Mesh
	{
	private:
		friend class MeshManager;
		real m_radius;				//!< The sphere radius
		unsigned int m_nbFaces;		//!< The subdivisions number
		/**
		 * Construstor, only the MeshManager can use it
		 *@param p_radius : [in] The wanted radius
		 *@param p_precision : [in] The wanted subdivisions number
		 *@param p_name : [in] The mesh name
		 */
		IcosaedricMesh( real p_radius=0.0, unsigned int p_precision=0,
						const String & p_name = C3DEmptyString);

	public:
		/**
		 * Constructor
		 */
		~IcosaedricMesh();
		/**
		 * Generates the icosaedron points
		 */
		virtual void GeneratePoints();
		/**
		 * Modifies the icosaedron caracteristics then rebuilds it
		 *@param p_radius : [in] The new wanted radius
		 *@param p_precision : [in] The new wanted subdivisions number
		 */
		void Modify( real p_radius, unsigned int p_precision);
		/**
		 * Defines the normals for the submesh
		 */
		virtual void SetNormals( bool p_bReverted = false);

	public:
		/**@name Accessors */
		//@{
		inline unsigned int	GetNbFaces	()const {return m_nbFaces;}
		inline real		GetRadius	()const {return m_radius;}
		//@}

		inline friend std::ostream & operator <<( std::ostream & o, const IcosaedricMesh & c) { return o << "Icosaedre(" << c.m_nbFaces << "," << c.m_radius << ")";}
	};
}

#endif
