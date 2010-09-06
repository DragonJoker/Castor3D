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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Module_Geometry___
#define ___C3D_Module_Geometry___

#include "../Module_General.h"



namespace Castor3D
{
	class Arc;
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
	class Subdiviser;
	class LoopSubdiviser;
	class PNTrianglesSubdiviser;

	struct FaceGroup;

	//! The structure managing Face and Angle
	/*!
	This is used to compute the smoothing groups
	*/
	struct FaceAndAngle
	{
		float m_angle;
		Face * m_face;
		int m_index;
		Vector3f * m_vertex1;
		Vector3f * m_vertex2;
	};

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
	/**
	 * Computes the center of a face, using the barycentre with the same weights
	 *@param p_face : The face which we want to compute the center
	 *@return The computed face center
	 */
	Vector3f * GetFaceCenter( Face * p_face);
	/**
	 * Set the texture coordinates for the first face vertex 
	 *@param p_face : The face
	 *@param x,y : The coordinates
	 */
	void SetTexCoordV1( Face * p_face, float x, float y);
	/**
	 * Set the texture coordinates for the second face vertex 
	 *@param p_face : The face
	 *@param x,y : The coordinates
	 */
	void SetTexCoordV2( Face * p_face, float x, float y);
	/**
	 * Set the texture coordinates for the third face vertex 
	 *@param p_face : The face
	 *@param x,y : The coordinates
	 */
	void SetTexCoordV3( Face * p_face, float x, float y);
	/**
	 * Writes a face in a file
	 *@param p_face : The face to write
	 *@param p_file : The file to write in
	 */
	bool WriteFace( Face * p_face, General::Utils::FileIO & p_file);
	/**
	 * Reads a face from a file
	 *@param p_face : The face to read
	 *@param p_file : The file to read from
	 */
	bool ReadFace( Face * p_face, General::Utils::FileIO & p_file);
	/**
	 * Writes a vertex in a file
	 *@param p_vertex : A pointer to the vertex to write
	 *@param p_file : The file to write in
	 */
	bool WriteVertex( const Vector3f * p_vertex, General::Utils::FileIO & p_file);
	/**
	 * Reads a vertex from a file
	 *@param p_vertex : A pointer to the vertex to read
	 *@param p_file : The file to read from
	 */
	bool ReadVertex( Vector3f * p_vertex, General::Utils::FileIO & p_file);
	/**
	 * Writes a vertex in a file
	 *@param p_vertex : The vertex to write
	 *@param p_file : The file to write in
	 */
	bool WriteVertex( const Vector3f & p_vertex, General::Utils::FileIO & p_file);
	/**
	 * Reads a vertex from a file
	 *@param p_vertex : The vertex to read
	 *@param p_file : The file to read from
	 */
	bool ReadVertex( Vector3f & p_vertex, General::Utils::FileIO & p_file);

	//! Face array
	typedef std::vector	<Face>						FaceArray;
	//! Face pointer array
	typedef std::vector	<Face *>					FacePtrArray;
	//! FaceAndAngle attay
	typedef std::vector	<FaceAndAngle>				FaceAndAngleArray;
	//! FaceGroup pointer array
	typedef std::vector <FaceGroup *>				FaceGroupPtrArray;
	//! Smoothing group pointer array
	typedef std::vector	<SmoothingGroup *>			SmoothGroupPtrArray;
	//! Smoothing group pointer array
	typedef std::map	<size_t, SmoothingGroup *>	SmoothGroupPtrMap;
	//! Submesh pointer array
	typedef std::vector <Submesh *>					SubmeshPtrArray;
	//! Geometry pointer array
	typedef std::vector <Geometry *>				GeometryPtrArray;
	//! Mesh pointer array
	typedef std::map	<String, Mesh *>			MeshPtrMap;
	//! MovableObject pointer map, sorted by name
	typedef std::map	<String, MovableObject *>	MovableObjectStrMap;
	//! Geometry pointer map, sorted by name
	typedef std::map	<String, Geometry *>		GeometryStrMap;
}



#endif

