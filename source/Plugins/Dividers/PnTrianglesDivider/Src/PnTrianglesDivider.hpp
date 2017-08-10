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
#ifndef ___C3D_PnTrianglesDivider___
#define ___C3D_PnTrianglesDivider___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Event/Frame/FrameListener.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Subdivider.hpp>

#include <Math/PlaneEquation.hpp>

#pragma warning( pop )

namespace PnTriangles
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		The PN Triangle bezier patch
	*/
	struct Patch
	{
		Patch( castor::PlaneEquation< double > const & p_p1, castor::PlaneEquation< double > const & p_p2, castor::PlaneEquation< double > const & p_p3 );

		castor::Point3d b300;
		castor::Point3d b030;
		castor::Point3d b003;
		castor::Point3d b201;
		castor::Point3d b210;
		castor::Point3d b120;
		castor::Point3d b021;
		castor::Point3d b102;
		castor::Point3d b012;
		castor::Point3d b111;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		Subdivider using PN Triangles subdivision algorithm
	*/
	class Subdivider : public castor3d::Subdivider
	{
	public:
		Subdivider();
		virtual ~Subdivider();

		static castor3d::SubdividerUPtr create();
		/**
		 *\copydoc		castor3d::Subdivider::Cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::Subdivider::Subdivide
		 */
		void subdivide( castor3d::SubmeshSPtr p_submesh, int p_occurences, bool p_generateBuffers = true, bool p_threaded = false )override;

	private:
		/**
		 *\copydoc		castor3d::Subdivider::doSubdivide
		 */
		void doSubdivide()override;
		void doComputeFaces( double u0, double v0, double u2, double v2, int p_occurences, Patch const & p_patch );
		castor3d::BufferElementGroupSPtr doComputePoint( double u, double v, Patch const & p_patch );

	public:
		static castor::String const Name;
		static castor::String const Type;

	private:
		int m_occurences;
	};
}

#endif
