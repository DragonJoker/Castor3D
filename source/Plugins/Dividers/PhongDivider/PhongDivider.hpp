/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PHONG_DIVIDER___
#define ___C3D_PHONG_DIVIDER___

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

namespace Phong
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

		Plane const & pi;
		Plane const & pj;
		Plane const & pk;
	};
	/**
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

		static castor3d::MeshSubdividerUPtr create();
		/**
		 *\copydoc		castor3d::Subdivider::Cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::Subdivider::Subdivide
		 */
		void subdivide( castor3d::SubmeshSPtr submesh
			, int occurences
			, bool generateBuffers
			, bool threaded )override;

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
		void doComputeFaces( float u0
			, float v0
			, float u2
			, float v2
			, int occurences
			, Patch const & patch );
		castor3d::SubmeshVertex & doComputePoint( float u
			, float v
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
