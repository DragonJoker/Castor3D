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
#ifndef ___C3D_Pave___
#define ___C3D_Pave___

#include "Mesh.h"

namespace Castor3D
{
	//! The cube representation
	/*!
	The calling of cube is abusive because it has parametrable dimensions (width, height and depth)
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API CubicMesh : public Mesh 
	{
	private:
		friend class MeshManager;
		float m_height;				//!< The cube height
		float m_width;				//!< The cube width
		float m_depth;				//!< The cube depth
		/**
		 * Constructor, only MeshManager can use it
		 *@param p_width : [in] The wanted cube width
		 *@param p_height : [in] The wanted cube height
		 *@param p_depth : [in] The wanted cube depth
		 *@param p_name : [in] The mesh name
		 */
		CubicMesh( float p_width = 0.0, float p_height = 0.0, float p_depth = 0.0,
				   const String & p_name = C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~CubicMesh();
		/**
		 * Generates the cube points
		 */
		virtual void GeneratePoints();
		/**
		 * Modifies the cube caracteristics then rebuild it
		 *@param p_width : [in] The new wanted cube width
		 *@param p_height : [in] The new wanted cube height
		 *@param p_depth : [in] The new wanted cube depth
		 */
		void Modify( float p_width, float p_height, float p_depth);

	public:
		inline float GetHeight	()const {return m_height;}
		inline float GetWidth	()const {return m_width;}
		inline float GetDepth	()const {return m_depth;}

		inline friend std::ostream & operator <<( std::ostream & o, const CubicMesh & c) { return o << "Pave(" << c.m_height << "," << c.m_width << "," << c.m_depth << ")";}
	};
}

#endif
