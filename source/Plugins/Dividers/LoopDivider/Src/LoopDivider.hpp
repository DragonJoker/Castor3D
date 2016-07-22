/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_LOOP_DIVIDER_H___
#define ___C3D_LOOP_DIVIDER_H___

#include "LoopPrerequisites.hpp"

namespace Loop
{
	//! Loop subdivision algorithm Subdivider
	/*!
	This class implements the Loop subdivision algorithm
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Subdivider : public Castor3D::Subdivider
	{
	public:
		Subdivider();
		virtual ~Subdivider();

		static Castor3D::SubdividerUPtr Create();
		/**
		 *\copydoc		Castor3D::Subdivider::Cleanup
		 */
		void Cleanup()override;
		/**
		 * Creates and adds a vertex to my list
		 *\param[in]	x	The vertex X coordinate
		 *\param[in]	y	The vertex Y coordinate
		 *\param[in]	z	The vertex Z coordinate
		 *\return	The created vertex
		 */
		VertexSPtr AddPoint( Castor::real x, Castor::real y, Castor::real z );
		/**
		 * Adds a vertex to my list
		 *\param[in]	p_v	The vertex to add
		 *\return	The vertex
		 */
		VertexSPtr AddPoint( Castor::Point3r const & p_v );
		/**
		 * Creates and adds a vertex to my list
		 *\param[in]	p_v	The vertex coordinates
		 *\return	The created vertex
		 */
		VertexSPtr AddPoint( Castor::real * p_v );

		inline Castor3D::SubmeshSPtr GetSubmesh()const
		{
			return m_submesh;
		}

	private:
		/**
		 *\copydoc		Castor3D::Subdivider::DoInitialise
		 */
		void DoInitialise()override;
		/**
		 *\copydoc		Castor3D::Subdivider::DoSubdivide
		 */
		void DoSubdivide()override;
		void DoDivide();
		void DoAverage();

	public:
		static Castor::String const Name;
		static Castor::String const Type;

	private:
		VertexPtrUIntMap m_mapVertex;
		FaceEdgesPtrArray m_facesEdges;
	};
}

#endif
