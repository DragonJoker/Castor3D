/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LOOP_PREREQUISITES_H___
#define ___C3D_LOOP_PREREQUISITES_H___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3DPrerequisites.hpp>

#include <Event/Frame/FrameListener.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/SubmeshComponent/Face.hpp>
#include <Mesh/Subdivider.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>

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
