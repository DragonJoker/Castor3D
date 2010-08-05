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
#ifndef ___C3D_Submesh___
#define ___C3D_Submesh___

#include "../../render_system/Module_Render.h"
#include "../../material/Module_Material.h"
#include "../basic/SmoothingGroup.h"


namespace Castor3D
{
	//! The submesh representation
	/*!
	A submesh holds its buffers (vertex, normas and texture) its smoothgroups and its combobox
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API Submesh
	{
		friend class Mesh;
		friend class IcosaedricMesh;
		friend class TorusMesh;
		friend class SphericMesh;
		friend class PlaneMesh;
		friend class CylindricMesh;
		friend class CubicMesh;
		friend class ConicMesh;

	protected:
		Material * m_material;						//!< The material of this submesh
		SmoothGroupPtrArray m_smoothGroups;			//!< The smoothgroups (which hold the faces)

		ComboBox * m_box;							//!< The combo box container
		Sphere * m_sphere;							//!< The spheric container
		
		Vector3fPtrArray m_vertex;					//!< The vertex pointer array
		Vector3fPtrArray m_tangents;				//!< The vertex pointer array
		Vector3fPtrArray m_normals;					//!< The vertex pointer array

		SubmeshRenderer * m_renderer;				//!< The submesh renderer
		VertexBuffer * m_triangles;					//!< The triangles vertex buffer
		NormalsBuffer * m_trianglesNormals;			//!< The triangles normals buffer
		VertexAttribsBuffer * m_trianglesTangents;	//!< The triangles tangent buffer
		TextureBuffer * m_trianglesTexCoords;		//!< The triangles texture buffer
		VertexBuffer * m_lines;						//!< The lines vertex buffer
		NormalsBuffer * m_linesNormals;				//!< The lines normals buffer
		TextureBuffer * m_linesTexCoords;			//!< The lines texture buffer

		std::map < SubdivisionMode, Subdiviser *> m_subdivisers;

		NormalsMode m_normalsMode;

	public:
		/**
		 * Constructor
		 *@param p_renderer : [in] The SubmeshRenderer, may be OpenGL or Direct3D
		 *@param p_sgNumber : [in] The number of smoothgroups of this submesh
		 */
		Submesh( SubmeshRenderer * p_renderer, size_t p_sgNumber=0);
		/**
		 * Destructor
		 */
		~Submesh();
		/**
		 * Cleans the submesh and the renderer
		 */
		void Cleanup();
		/**
		* Sets this geometry's material
		*@param p_matName : [in] the material name
		*/
		void SetMaterial( const String & p_matName);
		/**
		* @return This geometry's material, empty string if none
		*/
		String GetMaterialName()const;
		/**
		 * Computes the combo box
		 */
		void ComputeContainers();
		/**
		 * Returns the smoothing group at the given index, NULL if none
		 *@param p_index : [in] The index of the wanted smoothgroup
		 *@return The found SmoothingGroup, NULL if none
		 */
		SmoothingGroup * GetSmoothGroup( size_t p_index)const;
		/**
		 * Returns the total number of faces of this submesh
		 *@return The faces number
		 */
		size_t GetNbFaces()const;
		/**
		 * Tests if the given Vector3f is in mine
		 *@param p_vertex : [in] The vertex to test
		 *@return The index of the vertex equal to parameter, -1 if not found
		 */
		int IsInMyVertex( const Vector3f & p_vertex);
		/**
		 * Initialises the face normals
		 */
		void SetFlatNormals();
		/**
		 * Initialises the vertex normals
		 */
		void SetSmoothNormals();
		/**
		 * Initialises all the normals
		 */
		void SetNormals();
		/**
		 * Initialises vertex and texture buffers
		 */
		void CreateBuffers();
		/**
		 * Creates and adds a vertex to my list
		 *@param x : [in] The vertex X coordinate
		 *@param y : [in] The vertex Y coordinate
		 *@param z : [in] The vertex Z coordinate
		 *@return The created vertex
		 */
		Vector3f * AddVertex( float x, float y, float z);
		Vector3f * AddVertex( Vector3f * p_v);
		/**
		 * Creates and adds a face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex
		 *@param b : [in] The first face's vertex
		 *@param c : [in] The first face's vertex
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		Face * AddFace( Vector3f * a, Vector3f * b, Vector3f * c, size_t p_sgIndex);
		/**
		 * Creates and adds a face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex index
		 *@param b : [in] The first face's vertex index
		 *@param c : [in] The first face's vertex index
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		Face * AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex);
		/**
		 * Adds a smoothing group to the submesh
		 */
		SmoothingGroup * AddSmoothingGroup();
		/**
		 * Generates the vertex and texture buffers
		 */
		void GenerateBuffers();
		/**
		 * Adds a face to the vertex and texture buffers
		 *@param p_face : [in] The face to add
		 *@param p_trianglesIndex : [in/out] The current triangle's vertex index
		 *@param p_linesIndex : [in/out] The current line's vertex index
		 *@param p_trianglesTexIndex : [in/out] The current triangle's texture index
		 *@param p_linesTexIndex : [in/out] The current line's texture index
		 */
		void SetBuffersForFace( Face * p_face,
								unsigned int & p_trianglesIndex,
								unsigned int & p_linesIndex,
								unsigned int & p_trianglesTexIndex,
								unsigned int & p_linesTexIndex);

		void SetTangentBufferForFace( Face * p_face, unsigned int & p_trianglesTanIndex);
		/**
		 * Fills the normals buffer of the given mode
		 *@param p_nm : [in] The wanted NormalsMode
		 */
		void CreateNormalsBuffer( NormalsMode p_nm);
		/**
		 * Clones this submesh and returns the clone
		 *@return The clone
		 */
		Submesh * Clone();
		/**
		* Subdivides this mesh, in the given mode, centered on the given center, auto-center if NULL
		* auto-center computes the center for each face from it's vertices normals
		*/
		void Subdivide( SubdivisionMode p_mode, Vector3f * p_center=NULL);
		/**
		 * Writes the submesh in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		virtual bool Write( General::Utils::File & p_file)const;
		/**
		 * Reads the submesh from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		virtual bool Read( General::Utils::File & p_file);
		/**
		 * 
		 */
		void InvertNormals();
		/**
		 * 
		 */
		void ComputeFacesFromPolygonVertex();
		/**
		 * 
		 */	
		void GetSmoothGroups( SmoothGroupPtrArray & p_array);

	protected:
		void _setBuffersForVertex( unsigned int & p_trianglesIndex,
									unsigned int & p_linesIndex,
									unsigned int & p_trianglesTexIndex,
									unsigned int & p_linesTexIndex,
									Vector3f * p_v1, Vector3f * p_v2,
									const Point3D<float> & p_texCoord1, const Point3D<float> & p_texCoord2);
		void _setTangentBufferForVertex( unsigned int & p_trianglesTanIndex, Vector3f * p_v1, Vector3f * p_tangent);
		void _setBufferSmoothNormals( Face * p_face);
		void _setBufferFlatNormals( Face * p_face);
		void _initBuffers();
		/**
		* This subdivision mode using PN Triangles surface subdivision
		*/
		void _subdividePNTriangles( Vector3f * p_center=NULL);
		/**
		 * This subdivision mode using Loop surface subdivision
		 */
		void _subdivideLoop( Vector3f * p_center=NULL);

	private:
		void _addFace( Face * p_face, size_t p_sgIndex)						{ m_smoothGroups[p_sgIndex]->m_faces.push_back( p_face); }

	public:
		inline void			SetMaterial		( Material * p_mat)				{ m_material = p_mat; }
		inline void			AddSmoothGroup	( SmoothingGroup * p_group)		{ m_smoothGroups.push_back( p_group); }

		inline size_t				GetNbSmoothGroups		()const			{ return m_smoothGroups.size(); }
		inline size_t				GetNbVertex				()const			{ return m_vertex.size(); }
		inline Vector3f *			GetVertex				(size_t i)const	{ return (i < m_vertex.size() ? m_vertex[i] : NULL); }
		inline SubmeshRenderer *	GetRenderer				()const			{ return m_renderer; }
		inline Material *			GetMaterial				()const			{ return m_material; }
		inline ComboBox *			GetComboBox				()const			{ return m_box; }
		inline Sphere *				GetSphere				()const			{ return m_sphere; }
		inline Vector3fPtrArray *	GetVertices				()				{ return & m_vertex; }
		inline NormalsMode			GetNormalsMode			()const			{ return m_normalsMode; }
		inline SmoothGroupPtrArray *GetSmoothGroups			()				{ return & m_smoothGroups; }
	};
}

#endif
