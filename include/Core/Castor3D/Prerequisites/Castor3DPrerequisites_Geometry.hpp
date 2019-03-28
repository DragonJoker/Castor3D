/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_GEOMETRY_H___
#define ___C3D_PREREQUISITES_GEOMETRY_H___

namespace castor3d
{
	/**@name Geometry */
	//@{

	struct FaceGroup;
	class Face;
	class Graph;
	class Mesh;
	class MeshGenerator;
	class Submesh;
	class SubmeshComponent;
	class InstantiationComponent;
	class BonesComponent;
	class BonesInstantiationComponent;
	class MorphComponent;
	class IndexMapping;
	class TriFaceMapping;
	class Cone;
	class Cylinder;
	class Icosahedron;
	class Cube;
	class Plane;
	class Projection;
	class Sphere;
	class Torus;
	class Geometry;
	class MovableObject;
	class Subdivider;
	class Bone;
	class Skeleton;
	struct SubmeshVertex;
	struct VertexBoneData;

	CU_DeclareSmartPtr( Face );
	CU_DeclareSmartPtr( Graph );
	CU_DeclareSmartPtr( Mesh );
	CU_DeclareSmartPtr( MeshGenerator );
	CU_DeclareSmartPtr( Submesh );
	CU_DeclareSmartPtr( SubmeshComponent );
	CU_DeclareSmartPtr( InstantiationComponent );
	CU_DeclareSmartPtr( BonesInstantiationComponent );
	CU_DeclareSmartPtr( IndexMapping );
	CU_DeclareSmartPtr( Cone );
	CU_DeclareSmartPtr( Cylinder );
	CU_DeclareSmartPtr( Icosahedron );
	CU_DeclareSmartPtr( Cube );
	CU_DeclareSmartPtr( Plane );
	CU_DeclareSmartPtr( Projection );
	CU_DeclareSmartPtr( Sphere );
	CU_DeclareSmartPtr( Torus );
	CU_DeclareSmartPtr( Geometry );
	CU_DeclareSmartPtr( MovableObject );
	CU_DeclareSmartPtr( Subdivider );
	CU_DeclareSmartPtr( FaceGroup );
	CU_DeclareSmartPtr( Bone );
	CU_DeclareSmartPtr( Skeleton );

	class MeshFactory;
	CU_DeclareSmartPtr( MeshFactory );

	//! Face array
	CU_DeclareVector( Face, Face );
	//! Face pointer array
	CU_DeclareVector( FaceSPtr, FacePtr );
	//! FaceGroup pointer array
	CU_DeclareVector( FaceGroupSPtr, FaceGroupPtr );
	//! Submesh pointer array
	CU_DeclareVector( SubmeshSPtr, SubmeshPtr );
	//! Geometry pointer array
	CU_DeclareVector( GeometrySPtr, GeometryPtr );
	//! SubmeshVertex array.
	CU_DeclareVector( SubmeshVertex, SubmeshVertex );
	//! Mesh pointer array
	CU_DeclareMap( castor::String, MeshSPtr, MeshPtrStr );
	//! MovableObject pointer map, sorted by name
	CU_DeclareMap( castor::String, MovableObjectSPtr, MovableObjectPtrStr );
	//! Geometry pointer map, sorted by name
	CU_DeclareMap( castor::String, GeometrySPtr, GeometryPtrStr );
	CU_DeclareMap( uint32_t, int, IntUInt );
	CU_DeclareMap( castor::String, SubmeshSPtr, SubmeshPtrStr );
	CU_DeclareMap( castor::String, int, IntStr );
	//! Bone pointer array
	CU_DeclareVector( BoneSPtr, BonePtr );
	//! Bone pointer map sorted by name
	CU_DeclareMap( castor::String, BoneSPtr, BonePtrStr );
	CU_DeclareMap( castor::String, SubmeshComponentSPtr, SubmeshComponentStr );

	//@}
}

#endif
