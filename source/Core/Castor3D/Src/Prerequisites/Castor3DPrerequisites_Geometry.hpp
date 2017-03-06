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
#ifndef ___C3D_PREREQUISITES_GEOMETRY_H___
#define ___C3D_PREREQUISITES_GEOMETRY_H___

namespace Castor3D
{
	/**@name Geometry */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Mesh type enumeration
	\~french
	\brief		Enumération des types de maillages
	*/
	enum class MeshType
		: uint8_t
	{
		//!\~english Custom mesh type => User defined vertex, imported mesh...	\~french Type personnalisé => Défini par l'utilisateur, importé...
		eCustom,
		//!\~english Cone mesh type	\~french Cône
		eCone,
		//!\~english Cylinder mesh type	\~french Cylindre
		eCylinder,
		//!\~english Rectangular faces sphere mesh type	\~french Sphère à faces rectanglaires
		eSphere,
		//!\~english Cube mesh type	\~french Cube
		eCube,
		//!\~english Torus mesh type	\~french Torre
		eTorus,
		//!\~english Plane mesh type	\~french Plan
		ePlane,
		//!\~english Triangular faces sphere mesh type	\~french Sphère à faces triangulaires
		eIcosahedron,
		//!\~english Projection mesh type	\~french Projection
		eProjection,
		CASTOR_SCOPED_ENUM_BOUNDS( eCustom )
	};

	class Vertex;
	class VertexLayout;
	class BufferElementGroup;

	DECLARE_SMART_PTR( Vertex );
	DECLARE_SMART_PTR( BufferElementGroup );

	template< typename T > class Pattern;
	typedef Pattern< Vertex > VertexPattern;
	typedef Pattern< Castor::Point3r > Point3rPattern;
	typedef Pattern< VertexSPtr > VertexPtrPattern;
	typedef Pattern< Castor::Point3rSPtr > Point3rPtrPattern;

	struct FaceGroup;
	class Face;
	class Graph;
	class Mesh;
	class MeshGenerator;
	class Submesh;
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
	class BonedVertex;
	struct VertexBoneData;

	DECLARE_SMART_PTR( VertexPattern );
	DECLARE_SMART_PTR( Point3rPattern );
	DECLARE_SMART_PTR( VertexPtrPattern );
	DECLARE_SMART_PTR( Point3rPtrPattern );
	DECLARE_SMART_PTR( Face );
	DECLARE_SMART_PTR( Graph );
	DECLARE_SMART_PTR( Mesh );
	DECLARE_SMART_PTR( MeshGenerator );
	DECLARE_SMART_PTR( Submesh );
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
	DECLARE_SMART_PTR( BonedVertex );

	class MeshFactory;
	DECLARE_SMART_PTR( MeshFactory );

	//! Vertex array
	DECLARE_VECTOR( Vertex, Vertex );
	//! Vertex pointer array
	DECLARE_VECTOR( BufferElementGroupSPtr, VertexPtr );
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
	//! Vertex pointer list
	DECLARE_LIST( VertexSPtr, VertexPtr );
	//! Mesh pointer array
	DECLARE_MAP( Castor::String, MeshSPtr, MeshPtrStr );
	//! MovableObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, MovableObjectSPtr, MovableObjectPtrStr );
	//! Geometry pointer map, sorted by name
	DECLARE_MAP( Castor::String, GeometrySPtr, GeometryPtrStr );
	DECLARE_MAP( uint32_t, int, IntUInt );
	DECLARE_MAP( Castor::String, SubmeshSPtr, SubmeshPtrStr );
	DECLARE_MAP( Castor::String, int, IntStr );
	//! Bone pointer array
	DECLARE_VECTOR( BoneSPtr, BonePtr );
	//! Bone pointer map sorted by name
	DECLARE_MAP( Castor::String, BoneSPtr, BonePtrStr );
	//! BonedVertex pointer array
	DECLARE_VECTOR( BonedVertexSPtr, BonedVertexPtr );

	//@}
}

#endif
