/*
See LICENSE file in root folder
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
		 * Creates and adds a vertex to my list
		 *\param[in]	x	The vertex X coordinate
		 *\param[in]	y	The vertex Y coordinate
		 *\param[in]	z	The vertex Z coordinate
		 *\return	The created vertex
		 */
		VertexSPtr addPoint( castor::real x, castor::real y, castor::real z );
		/**
		 * adds a vertex to my list
		 *\param[in]	p_v	The vertex to add
		 *\return	The vertex
		 */
		VertexSPtr addPoint( castor::Point3r const & p_v );
		/**
		 * Creates and adds a vertex to my list
		 *\param[in]	p_v	The vertex coordinates
		 *\return	The created vertex
		 */
		VertexSPtr addPoint( castor::real * p_v );

		inline castor3d::SubmeshSPtr getSubmesh()const
		{
			return m_submesh;
		}

	private:
		/**
		 *\copydoc		castor3d::Subdivider::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::Subdivider::doSubdivide
		 */
		void doSubdivide()override;
		void doDivide();
		void doAverage();

	public:
		static castor::String const Name;
		static castor::String const Type;

	private:
		VertexPtrUIntMap m_mapVertex;
		FaceEdgesPtrArray m_facesEdges;
	};
}

#endif
