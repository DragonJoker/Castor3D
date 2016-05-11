/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
	typedef enum eMESH_TYPE
		: uint8_t
	{
		//!\~english Custom mesh type => User defined vertex, imported mesh...	\~french Type personnalisé => Défini par l'utilisateur, importé...
		eMESH_TYPE_CUSTOM,
		//!\~english Cone mesh type	\~french Cône
		eMESH_TYPE_CONE,
		//!\~english Cylinder mesh type	\~french Cylindre
		eMESH_TYPE_CYLINDER,
		//!\~english Rectangular faces sphere mesh type	\~french Sphère à faces rectanglaires
		eMESH_TYPE_SPHERE,
		//!\~english Cube mesh type	\~french Cube
		eMESH_TYPE_CUBE,
		//!\~english Torus mesh type	\~french Torre
		eMESH_TYPE_TORUS,
		//!\~english Plane mesh type	\~french Plan
		eMESH_TYPE_PLANE,
		//!\~english Triangular faces sphere mesh type	\~french Sphère à faces triangulaires
		eMESH_TYPE_ICOSAHEDRON,
		//!\~english Projection mesh type	\~french Projection
		eMESH_TYPE_PROJECTION,
		CASTOR_ENUM_BOUNDS( eMESH_TYPE, eMESH_TYPE_CUSTOM )
	}	eMESH_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Enumerates supported normal generation
	\~french
	\brief		Enumération des types de génération de normales supportés
	*/
	typedef enum eNORMAL
		: uint8_t
	{
		//!\~english Flat (face) normals	\~french Normales plates (par face)
		eNORMAL_FLAT,
		//!\~english Smooth normals	\~french Normales smooth
		eNORMAL_SMOOTH,
		CASTOR_ENUM_BOUNDS( eNORMAL, eNORMAL_FLAT )
	}	eNORMAL;

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

	//@}
}

#endif
