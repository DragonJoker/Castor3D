/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PnTrianglesDivider___
#define ___C3D_PnTrianglesDivider___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Model/Mesh/Subdivider.hpp>

#include <CastorUtils/Math/PlaneEquation.hpp>

#pragma warning( pop )

namespace PnTriangles
{
	struct Plane
	{
		castor::PlaneEquation plane;
		castor::Point3f point;
	};
	/**
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		The PN Triangle bezier patch
	*/
	struct Patch
	{
		Patch( Plane const & p1, Plane const & p2, Plane const & p3 );

		castor::Point3f b300;
		castor::Point3f b030;
		castor::Point3f b003;
		castor::Point3f b201;
		castor::Point3f b210;
		castor::Point3f b120;
		castor::Point3f b021;
		castor::Point3f b102;
		castor::Point3f b012;
		castor::Point3f b111;
	};
	/**
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		Subdivider using PN Triangles subdivision algorithm
	*/
	class Subdivider
		: public castor3d::MeshSubdivider
	{
	public:
		Subdivider();
		~Subdivider()override;

		static castor3d::MeshSubdividerUPtr create();
		/**
		 *\copydoc		castor3d::MeshSubdivider::Cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::MeshSubdivider::Subdivide
		 */
		void subdivide( castor3d::SubmeshSPtr submesh
			, int occurences
			, bool generateBuffers
			, bool p_threaded )override;

	private:
		/**
		 *\copydoc		castor3d::MeshSubdivider::doSubdivide
		 */
		void doSubdivide()override;
		/**
		 *\copydoc		castor3d::MeshSubdivider::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::MeshSubdivider::doAddGeneratedFaces
		 */
		void doAddGeneratedFaces()override;
		void doComputeFaces( double u0
			, double v0
			, double u2
			, double v2
			, int occurences
			, Patch const & patch );
		castor3d::SubmeshVertex & doComputePoint( double u
			, double v
			, Patch const & patch );

	public:
		static castor::String const Name;
		static castor::String const Type;

	private:
		int m_occurences;
		std::shared_ptr< castor3d::TriFaceMapping > m_indexMapping;
	};
}

#endif
