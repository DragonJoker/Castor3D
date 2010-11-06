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
#ifndef ___C3D_Module_Geometry___
#define ___C3D_Module_Geometry___

#include "../Module_General.h"

namespace Castor3D
{
	class Pattern;
	class Face;
	class Graph;
	class SmoothingGroup;
	class MeshManager;
	class Mesh;
	class Submesh;
	class ConicMesh;
	class CylindricMesh;
	class IcosaedricMesh;
	class CubicMesh;
	class PlaneMesh;
	class SphericMesh;
	class TorusMesh;
	class Geometry;
	class MovableObject;
	class Edge;
	class FaceEdges;
	class Subdiviser;
	class LoopSubdiviser;
	class PNTrianglesSubdiviser;

	struct FaceGroup;

	typedef Templates::SharedPtr<Pattern>				PatternPtr;
	typedef Templates::SharedPtr<Face>					FacePtr;
	typedef Templates::SharedPtr<Graph>					GraphPtr;
	typedef Templates::SharedPtr<SmoothingGroup>		SmoothingGroupPtr;
	typedef Templates::SharedPtr<Mesh>					MeshPtr;
	typedef Templates::SharedPtr<Submesh>				SubmeshPtr;
	typedef Templates::SharedPtr<ConicMesh>				ConicMeshPtr;
	typedef Templates::SharedPtr<CylindricMesh>			CylindricMeshPtr;
	typedef Templates::SharedPtr<IcosaedricMesh>		IcosaedricMeshPtr;
	typedef Templates::SharedPtr<CubicMesh>				CubicMeshPtr;
	typedef Templates::SharedPtr<PlaneMesh>				PlaneMeshPtr;
	typedef Templates::SharedPtr<SphericMesh>			SphericMeshPtr;
	typedef Templates::SharedPtr<TorusMesh>				TorusMeshPtr;
	typedef Templates::SharedPtr<Geometry>				GeometryPtr;
	typedef Templates::SharedPtr<MovableObject>			MovableObjectPtr;
	typedef Templates::SharedPtr<Edge>					EdgePtr;
	typedef Templates::SharedPtr<FaceEdges>				FaceEdgesPtr;
	typedef Templates::SharedPtr<Subdiviser>			SubdiviserPtr;
	typedef Templates::SharedPtr<LoopSubdiviser>		LoopSubdiviserPtr;
	typedef Templates::SharedPtr<PNTrianglesSubdiviser>	PNTrianglesSubdiviserPtr;
	typedef Templates::SharedPtr<FaceGroup>				FaceGroupPtr;

	//! The normals mode enumerator
	/*!
	Actually, 2 Normals mode are implemented : Face and Smoothgroups
	*/
	typedef enum NormalsMode
	{
		nmFace			= 0,	//!< Face normals mode sets to each face vertex the same face normal
		nmSmoothGroups	= 1		//!< Smoothgroup normals mode sets to each face vertex a normal computed from it's position relative to his neighbour's one
	} NormalsMode;
	//! The subdivision mode enumerator
	/*!
	Actually, 2 divisions mode, depending on which kind of face we consider : triangles or quads
	*/
	typedef enum SubdivisionMode
	{
		SMPNTriangles,
		SMLoop,
		SMCatmull,
		SMDooSabin,
		SMMidEdge,
		SMSqrt3,
		SMPlane
	} SubdivisionMode;

	typedef C3DVector(	PatternPtr)								PatternPtrArray;		//!< Pattern pointer array
	typedef C3DVector(	Face)									FaceArray;				//!< Face array
	typedef C3DVector(	FacePtr)								FacePtrArray;			//!< Face pointer array
	typedef C3DVector(	FaceGroupPtr)							FaceGroupPtrArray;		//!< FaceGroup pointer array
	typedef C3DVector(	SmoothingGroupPtr)						SmoothGroupPtrArray;	//!< Smoothing group pointer array
	typedef C3DVector(	SubmeshPtr)								SubmeshPtrArray;		//!< Submesh pointer array
	typedef C3DVector(	GeometryPtr)							GeometryPtrArray;		//!< Geometry pointer array
	typedef C3DVector(	FaceEdgesPtr)							FaceEdgesPtrArray;
	typedef C3DMap(		String,				MeshPtr)			MeshPtrStrMap;			//!< Mesh pointer array
	typedef C3DMap(		String,				MovableObjectPtr)	MovableObjectPtrStrMap;	//!< MovableObject pointer map, sorted by name
	typedef C3DMap(		String,				GeometryPtr)		GeometryPtrStrMap;		//!< Geometry pointer map, sorted by name
	typedef C3DMap(		size_t,				SmoothingGroupPtr)	SmoothGroupPtrUIntMap;	//!< Smoothing group pointer array
	typedef C3DMap(		String,				SmoothingGroupPtr)	SmoothGroupPtrStrMap;
	typedef C3DMap(		VertexPtr,			EdgePtr)			EdgePtrMap;				//!< Map of edges, ordered by vertex
	typedef C3DMap(		VertexPtr,			EdgePtrMap)			EdgePtrMapVPtrMap;
	typedef C3DMap(		VertexPtr,			VertexPtr)			VertexPtrVPtrMap;
	typedef C3DMap(		VertexPtr,			int)				IntVPtrMap;
	typedef C3DMap(		SubdivisionMode,	SubdiviserPtr)		SubdiviserPtrModeMap;
	typedef C3DMap(		String,				SubmeshPtr)			SubmeshPtrStrMap;
}



#endif

