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

	DECLARE_SMART_PTR( Face );
	DECLARE_SMART_PTR( Graph );
	DECLARE_SMART_PTR( Mesh );
	DECLARE_SMART_PTR( MeshGenerator );
	DECLARE_SMART_PTR( Submesh );
	DECLARE_SMART_PTR( SubmeshComponent );
	DECLARE_SMART_PTR( InstantiationComponent );
	DECLARE_SMART_PTR( BonesInstantiationComponent );
	DECLARE_SMART_PTR( IndexMapping );
	DECLARE_SMART_PTR( Cone );
	DECLARE_SMART_PTR( Cylinder );
	DECLARE_SMART_PTR( Icosahedron );
	DECLARE_SMART_PTR( Cube );
	DECLARE_SMART_PTR( Plane );
	DECLARE_SMART_PTR( Projection );
	DECLARE_SMART_PTR( Sphere );
	DECLARE_SMART_PTR( Torus );
	DECLARE_SMART_PTR( Geometry );
	DECLARE_SMART_PTR( MovableObject );
	DECLARE_SMART_PTR( Subdivider );
	DECLARE_SMART_PTR( FaceGroup );
	DECLARE_SMART_PTR( Bone );
	DECLARE_SMART_PTR( Skeleton );

	class MeshFactory;
	DECLARE_SMART_PTR( MeshFactory );

	//! Face array
	DECLARE_VECTOR( Face, Face );
	//! Face pointer array
	DECLARE_VECTOR( FaceSPtr, FacePtr );
	//! FaceGroup pointer array
	DECLARE_VECTOR( FaceGroupSPtr, FaceGroupPtr );
	//! Submesh pointer array
	DECLARE_VECTOR( SubmeshSPtr, SubmeshPtr );
	//! Geometry pointer array
	DECLARE_VECTOR( GeometrySPtr, GeometryPtr );
	//! SubmeshVertex array.
	DECLARE_VECTOR( SubmeshVertex, SubmeshVertex );
	//! Mesh pointer array
	DECLARE_MAP( castor::String, MeshSPtr, MeshPtrStr );
	//! MovableObject pointer map, sorted by name
	DECLARE_MAP( castor::String, MovableObjectSPtr, MovableObjectPtrStr );
	//! Geometry pointer map, sorted by name
	DECLARE_MAP( castor::String, GeometrySPtr, GeometryPtrStr );
	DECLARE_MAP( uint32_t, int, IntUInt );
	DECLARE_MAP( castor::String, SubmeshSPtr, SubmeshPtrStr );
	DECLARE_MAP( castor::String, int, IntStr );
	//! Bone pointer array
	DECLARE_VECTOR( BoneSPtr, BonePtr );
	//! Bone pointer map sorted by name
	DECLARE_MAP( castor::String, BoneSPtr, BonePtrStr );
	DECLARE_MAP( castor::String, SubmeshComponentSPtr, SubmeshComponentStr );

	//@}
}

#endif
