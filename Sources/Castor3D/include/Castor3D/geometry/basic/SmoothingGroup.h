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
#ifndef ___C3D_SmoothingGroup___
#define ___C3D_SmoothingGroup___

namespace Castor3D
{
	//! Smoothing groups handler class
	/*!
	Smoothig groups are used to build vertex normals and also to sort vertexes in object sections
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API SmoothingGroup
	{
	public:
		size_t m_idGroup;		//!< The group ID
		FacePtrArray m_faces;	//!< The faces in the group

	public:
		/**
		 * Constructor
		 *@param p_id : [in] the group's id
		 */
		SmoothingGroup( size_t p_id=1);
		/**
		 * Destructor
		 */
		~SmoothingGroup();
		/**
		 * Computes the vertexes normals for a given number of vertexes
		 *@param p_nbVertex : [in] The number of vertex for which we generate normals
		 *@param p_normals : [in] The faces normals for all vertex
		 *@param p_tangents : [in] The faces tangents for all vertex
		 */
		void SetNormals( size_t p_nbVertex, const Point3rPtrArray & p_normals, const Point3rPtrArray & p_tangents);
	};
}

#endif
