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
#ifndef ___C3D_Submesh___
#define ___C3D_Submesh___

#include "../Module_Geometry.h"
#include "../../render_system/Module_Render.h"
#include "../../material/Module_Material.h"
#include "../../render_system/Renderable.h"

namespace Castor3D
{
	//! The submesh representation
	/*!
	A submesh holds its buffers (vertex, normas and texture) its smoothgroups and its combobox
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API Submesh : public Renderable<Submesh, SubmeshRenderer>
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
		Templates::WeakPtr<Material> m_material;			//!< The material
		String m_strMatName;
		SmoothGroupPtrArray m_smoothGroups;					//!< The smoothgroups (which hold the faces)

		ComboBoxPtr m_box;									//!< The combo box container
		SpherePtr m_sphere;									//!< The spheric container
		
		VertexPtrArray m_vertex;							//!< The vertex pointer array
		Point3rPtrArray m_tangents;							//!< The vertex pointer array
		Point3rPtrArray m_normals;							//!< The vertex pointer array

		VertexBufferPtr				m_triangles;			//!< The triangles vertex buffer
		NormalsBufferPtr			m_trianglesNormals;		//!< The triangles normals buffer
		VertexAttribsBufferRealPtr	m_trianglesTangents;	//!< The triangles tangent buffer
		TextureBufferPtr			m_trianglesTexCoords;	//!< The triangles texture buffer
		VertexBufferPtr				m_lines;				//!< The lines vertex buffer
		NormalsBufferPtr			m_linesNormals;			//!< The lines normals buffer
		TextureBufferPtr			m_linesTexCoords;		//!< The lines texture buffer

		SubdiviserPtrModeMap m_subdivisers;

		NormalsMode m_normalsMode;

	public:
		/**
		 * Constructor
		 *@param p_sgNumber : [in] The number of smoothgroups
		 */
		Submesh( size_t p_sgNumber=0);
		/**
		 * Destructor
		 */
		~Submesh();
		/**
		 * Cleans the submesh and the renderer
		 */
		void Cleanup();
		/**
		* Sets material
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
		SmoothingGroupPtr GetSmoothGroup( size_t p_index)const;
		/**
		 * Returns the total number of faces
		 *@return The faces number
		 */
		size_t GetNbFaces()const;
		/**
		 * Tests if the given Point3r is in mine
		 *@param p_vertex : [in] The vertex to test
		 *@return The index of the vertex equal to parameter, -1 if not found
		 */
		int IsInMyVertex( const Vertex & p_vertex);
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
		VertexPtr AddVertex( real x, real y, real z);
		/**
		 * Adds a vertex to my list
		 *@param p_v : [in] The vertex to add
		 *@return The vertex
		 */
		VertexPtr AddVertex( const Vertex & p_v);
		/**
		 * Adds a vertex to my list
		 *@param p_v : [in] The vertex to add
		 *@return The vertex
		 */
		VertexPtr AddVertex( VertexPtr p_v);
		/**
		 * Creates and adds a vertex to my list
		 *@param p_v : [in] The vertex coordinates
		 *@return The created vertex
		 */
		VertexPtr AddVertex( real * p_v);
		/**
		 * Creates and adds a face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex
		 *@param b : [in] The second face's vertex
		 *@param c : [in] The third face's vertex
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		FacePtr AddFace( VertexPtr a, VertexPtr b, VertexPtr c, size_t p_sgIndex);
		/**
		 * Creates and adds a face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex index
		 *@param b : [in] The second face's vertex index
		 *@param c : [in] The third face's vertex index
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		FacePtr AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex);
		/**
		 * Creates and adds a quad face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex
		 *@param b : [in] The second face's vertex
		 *@param c : [in] The third face's vertex
		 *@param d : [in] The fourth face's vertex
		 *@param p_ptMinUV : [in] Minimum UV
		 *@param p_ptMaxUV : [in] Maximum UV
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		void AddQuadFace( VertexPtr a, VertexPtr b, VertexPtr c, VertexPtr d, size_t p_sgIndex, const Point3r & p_ptMinUV = Point3r( real( 0), real( 0), real( 0)), const Point3r & p_ptMaxUV = Point3r( real( 1), real( 1), real( 1)));
		/**
		 * Creates and adds a quad face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex index
		 *@param b : [in] The second face's vertex index
		 *@param c : [in] The third face's vertex index
		 *@param d : [in] The fourth face's vertex index
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@param p_ptMinUV : [in] The UV of the bottom left corner
		 *@param p_ptMaxUV : [in] The UV of the top right corner
		 *@return The created face
		 */
		void AddQuadFace( size_t a, size_t b, size_t c, size_t d, size_t p_sgIndex, const Point3r & p_ptMinUV = Point3r( real( 0), real( 0), real( 0)), const Point3r & p_ptMaxUV = Point3r( real( 1), real( 1), real( 1)));
		/**
		 * Adds a smoothing group to the submesh
		 */
		SmoothingGroupPtr AddSmoothingGroup();
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
		void SetBuffersForFace( FacePtr p_face,
								unsigned int & p_trianglesIndex,
								unsigned int & p_linesIndex,
								unsigned int & p_trianglesTexIndex,
								unsigned int & p_linesTexIndex);

		void SetTangentBufferForFace( FacePtr p_face, unsigned int & p_trianglesTanIndex);
		/**
		 * Fills the normals buffer of the given mode
		 *@param p_nm : [in] The wanted NormalsMode
		 */
		void CreateNormalsBuffer( NormalsMode p_nm);
		/**
		 * Clones the submesh and returns the clone
		 *@return The clone
		 */
		SubmeshPtr Clone();
		/**
		* Subdivides the submesh, in the given mode, centered on the given center, auto-center if NULL
		* auto-center computes the center for each face from it's vertices normals
		*/
		void Subdivide( SubdivisionMode p_mode, Point3rPtr p_center=NULL);
		/**
		 * Writes the submesh in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		virtual bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads the submesh from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		virtual bool Read( Castor::Utils::File & p_file);
		/**
		 * 
		 */
		void ComputeFacesFromPolygonVertex();
		/**
		 * 
		 */	
		void GetSmoothGroups( SmoothGroupPtrArray & p_array);

		virtual void Apply( eDRAW_TYPE p_displayMode);

	protected:
		void _setBuffersForVertex( unsigned int & p_trianglesIndex,
									unsigned int & p_linesIndex,
									unsigned int & p_trianglesTexIndex,
									unsigned int & p_linesTexIndex,
									VertexPtr p_v1, VertexPtr p_v2,
									const Point3r & p_texCoord1, const Point3r & p_texCoord2);
		void _setTangentBufferForVertex( unsigned int & p_trianglesTanIndex, VertexPtr * p_v1, VertexPtr * p_tangent);
		void _setBufferSmoothNormals( FacePtr p_face);
		void _setBufferFlatNormals( FacePtr p_face);
		void _initBuffers();
		/**
		* This subdivision mode using PN Triangles surface subdivision
		*/
		void _subdividePNTriangles( Point3rPtr p_center=NULL);
		/**
		 * This subdivision mode using Loop surface subdivision
		 */
		void _subdivideLoop( Point3rPtr p_center=NULL);

	private:
		void _addFace( FacePtr p_face, size_t p_sgIndex);

	public:
		/**@name Accessors */
		//@{
		inline void SetMaterial		( MaterialPtr p_mat)			{ m_material = p_mat; }
		inline void AddSmoothGroup	( SmoothingGroupPtr p_group)	{ m_smoothGroups.push_back( p_group); }

		inline size_t					GetNbSmoothGroups		()const				{ return m_smoothGroups.size(); }
		inline size_t					GetNbVertex				()const				{ return m_vertex.size(); }
		inline VertexPtr				GetVertex				( size_t i)const	{ return (i < m_vertex.size() ? m_vertex[i] : NULL); }
		inline MaterialPtr				GetMaterial				()const				{ return m_material; }
		inline ComboBoxPtr				GetComboBox				()const				{ return m_box; }
		inline SpherePtr				GetSphere				()const				{ return m_sphere; }
		inline VertexPtrArray &			GetVertices				()					{ return m_vertex; }
		inline NormalsMode				GetNormalsMode			()const				{ return m_normalsMode; }
		inline SmoothGroupPtrArray	&	GetSmoothGroups			()					{ return m_smoothGroups; }
		//@}
	};
	//! The submesh renderer
	/*!
	Initialises the vertex, normals, textures buffers of a submesh, draws it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API SubmeshRenderer : public Renderer<Submesh, SubmeshRenderer>
	{
	protected:
		VertexBufferPtr				m_triangles;			//!< Pointer over geometry triangles vertex buffer
//		VertexAttribsBufferRealPtr	m_trianglesNormals;		//!< Pointer over geometry triangles normals buffer
		NormalsBufferPtr			m_trianglesNormals;		//!< Pointer over geometry triangles normals buffer
		VertexAttribsBufferRealPtr	m_trianglesTangents;	//!< Pointer over geometry triangles tangents buffer
		TextureBufferPtr			m_trianglesTexCoords;	//!< Pointer over geometry triangles texture coords buffer
		VertexBufferPtr				m_lines;				//!< Pointer over geometry lines vertex buffer
//		VertexAttribsBufferRealPtr	m_linesNormals;			//!< Pointer over geometry lines normals buffer
		NormalsBufferPtr			m_linesNormals;			//!< Pointer over geometry lines normals buffer
		TextureBufferPtr			m_linesTexCoords;		//!< Pointer over geometry lines texture coords buffer
		/**
		 * Constructor, only RenderSystem can use it
		 */
		SubmeshRenderer()
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~SubmeshRenderer(){ Cleanup(); }
		/**
		 * Cleans the renderer
		 */
		virtual void Cleanup(){}
		/**
		* Initialises the geometry's buffers
		*/
		virtual void Initialise() = 0;
		/**
		* Initialises the geometry's buffers
		*/
		virtual void Draw( eDRAW_TYPE p_mode, PassPtr p_pass) = 0;
		/**
		 * Shows the submesh vertices (enables its arrays)
		 *@param p_displayMode : [in] The wanted disply mode
		 */
		virtual void ShowVertex( eDRAW_TYPE p_displayMode) = 0;
		/**
		 * Hides the submesh vertices (disables its arrays)
		 */
		virtual void HideVertex() = 0;

	public:
		/**@name Accessors */
		//@{
		inline VertexBufferPtr				GetTriangles			()const { return m_triangles; }
//		inline VertexAttribsBufferRealPtr	GetTrianglesNormals		()const { return m_trianglesNormals; }
		inline NormalsBufferPtr				GetTrianglesNormals		()const { return m_trianglesNormals; }
		inline VertexAttribsBufferRealPtr	GetTrianglesTangents	()const { return m_trianglesTangents; }
		inline TextureBufferPtr				GetTrianglesTexCoords	()const { return m_trianglesTexCoords; }
		inline VertexBufferPtr				GetLines				()const { return m_lines; }
//		inline VertexAttribsBufferRealPtr	GetLinesNormals			()const { return m_linesNormals; }
		inline NormalsBufferPtr				GetLinesNormals			()const { return m_linesNormals; }
		inline TextureBufferPtr				GetLinesTexCoords		()const { return m_linesTexCoords; }
		//@}
	};
}

#endif
