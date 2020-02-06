/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
