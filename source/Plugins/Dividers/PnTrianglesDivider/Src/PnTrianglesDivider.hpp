/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PnTrianglesDivider___
#define ___C3D_PnTrianglesDivider___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Event/Frame/FrameListener.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/SubmeshComponent/Face.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Subdivider.hpp>

#include <Math/PlaneEquation.hpp>

#pragma warning( pop )

namespace PnTriangles
{
	struct Plane
	{
		castor::PlaneEquation plane;
		castor::Point3r point;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		The PN Triangle bezier patch
	*/
	struct Patch
	{
		Patch( Plane const & p_p1, Plane const & p_p2, Plane const & p_p3 );

		castor::Point3r b300;
		castor::Point3r b030;
		castor::Point3r b003;
		castor::Point3r b201;
		castor::Point3r b210;
		castor::Point3r b120;
		castor::Point3r b021;
		castor::Point3r b102;
		castor::Point3r b012;
		castor::Point3r b111;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		Subdivider using PN Triangles subdivision algorithm
	*/
	class Subdivider
		: public castor3d::Subdivider
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
		/**
		 *\copydoc		castor3d::Subdivider::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::Subdivider::doAddGeneratedFaces
		 */
		void doAddGeneratedFaces()override;
		void doComputeFaces( double u0, double v0, double u2, double v2, int p_occurences, Patch const & p_patch );
		castor3d::BufferElementGroupSPtr doComputePoint( double u, double v, Patch const & p_patch );

	public:
		static castor::String const Name;
		static castor::String const Type;

	private:
		int m_occurences;
		std::shared_ptr< castor3d::TriFaceMapping > m_indexMapping;
	};
}

#endif
