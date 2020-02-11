/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LOOP_PREREQUISITES_H___
#define ___C3D_LOOP_PREREQUISITES_H___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#include <Castor3D/Model/Mesh/Subdivider.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>
#include <Castor3D/Plugin/Plugin.hpp>

#pragma warning( pop )

namespace Loop
{
	class Subdivider;
	class Vertex;
	class Edge;
	class FaceEdges;

	CU_DeclareSmartPtr( Vertex );
	CU_DeclareSmartPtr( Edge );
	CU_DeclareSmartPtr( FaceEdges );

	CU_DeclareVector( FaceEdgesSPtr, FaceEdgesPtr );
	//! Map of edges, ordered by index
	CU_DeclareMap( uint32_t, EdgeSPtr, EdgePtrUInt );
	CU_DeclareMap( uint32_t, EdgePtrUIntMap, EdgePtrUIntMapUInt );
	CU_DeclareMap( uint32_t, VertexSPtr, VertexPtrUInt );
}

#endif
