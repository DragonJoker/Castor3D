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
#include "../basic/SmoothingGroup.h"

namespace Castor3D
{
	//! The submesh representation
	/*!
	A submesh holds its buffers (vertex, normas and texture) its smoothgroups and its combobox
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Submesh : public Renderable<Submesh, SubmeshRenderer>
	{
/*
		friend class Mesh;
		friend class IcosaedricMesh;
		friend class TorusMesh;
		friend class SphericMesh;
		friend class PlaneMesh;
		friend class CylindricMesh;
		friend class CubicMesh;
		friend class ConicMesh;
*/
	protected:
		Templates::WeakPtr<Material> m_material;			//!< The material
		String m_strMatName;
		SmoothGroupArray m_smoothGroups;					//!< The smoothgroups (which hold the faces)

		ComboBox m_box;										//!< The combo box container
		Sphere m_sphere;									//!< The spheric container
		
		Point3rArray m_vertex;								//!< The vertex pointer array

		VertexInfosBufferPtr		m_triangles;			//!< The triangles vertex buffer
		VertexInfosBufferPtr		m_lines;				//!< The lines vertex buffer

		SubdiviserPtrModeMap m_subdivisers;

		eNORMALS_MODE m_normalsMode;

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
		 * Returns the total number of faces
		 *@return The faces number
		 */
		size_t GetNbFaces()const;
		/**
		 * Tests if the given Point3r is in mine
		 *@param p_vertex : [in] The vertex to test
		 *@return The index of the vertex equal to parameter, -1 if not found
		 */
		int IsInMyVertex( const Point3r & p_vertex);
		/**
		 * Initialises the face normals
		 */
		void ComputeFlatNormals();
		/**
		 * Initialises the vertex normals
		 */
		void ComputeSmoothNormals();
		/**
		 * Initialises all the normals
		 */
		void ComputeNormals();
		/**
		 * Initialises vertex and texture buffers
		 */
		void InitialiseBuffers();
		/**
		 * Fills the normals buffer of the given mode
		 *@param p_nm : [in] The wanted NormalsMode
		 */
		void SetNormals( eNORMALS_MODE p_nm);
		/**
		 * Creates and adds a vertex to my list
		 *@param x : [in] The vertex X coordinate
		 *@param y : [in] The vertex Y coordinate
		 *@param z : [in] The vertex Z coordinate
		 *@return The created vertex
		 */
		Point3r & AddVertex( real x, real y, real z);
		/**
		 * Adds a vertex to my list
		 *@param p_v : [in] The vertex to add
		 *@return The vertex
		 */
		Point3r & AddVertex( const Point3r & p_v);
		/**
		 * Creates and adds a vertex to my list
		 *@param p_v : [in] The vertex coordinates
		 *@return The created vertex
		 */
		Point3r & AddVertex( real * p_v);
		/**
		 * Creates and adds a face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex index
		 *@param b : [in] The second face's vertex index
		 *@param c : [in] The third face's vertex index
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		Face & AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex);
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
		SmoothingGroup & AddSmoothingGroup();
		void AddSmoothGroup( const SmoothingGroup & p_group);
		/**
		 * Generates the vertex and texture buffers
		 */
		void GenerateBuffers();
		/**
		 * Clones the submesh and returns the clone
		 *@return The clone
		 */
		SubmeshPtr Clone();
		/**
		* Subdivides the submesh, in the given mode, centered on the given center, auto-center if NULL
		* auto-center computes the center for each face from it's vertices normals
		*/
		void Subdivide( SubdivisionMode p_mode, Point3r * p_center=NULL);
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
		void GetSmoothGroups( SmoothGroupArray & p_array);

		virtual void Render( eDRAW_TYPE p_displayMode);

	private:
		void _subdividePNTriangles( Point3r * p_center=NULL);
		void _subdivideLoop( Point3r * p_center=NULL);
		Face & _addFace( const Face & p_face, size_t p_sgIndex);

	public:
		/**@name Accessors */
		//@{
		inline void SetMaterial		( MaterialPtr p_mat)				{ m_material = p_mat; }

		inline size_t					GetNbSmoothGroups		()const				{ return m_smoothGroups.size(); }
		inline size_t					GetNbVertex				()const				{ return m_vertex.size(); }
		inline const Point3r &			GetVertex				( size_t i)const	{ CASTOR_ASSERT( i < m_vertex.size());return m_vertex[i]; }
		inline Point3r &				GetVertex				( size_t i)			{ CASTOR_ASSERT( i < m_vertex.size());return m_vertex[i]; }
		inline MaterialPtr				GetMaterial				()const				{ return m_material.expired() ? MaterialPtr() : m_material; }
		inline const ComboBox &			GetComboBox				()const				{ return m_box; }
		inline const Sphere &			GetSphere				()const				{ return m_sphere; }
		inline Point3rArray &			GetVertices				()					{ return m_vertex; }
		inline const Point3rArray &		GetVertices				()const				{ return m_vertex; }
		inline eNORMALS_MODE			GetNormalsMode			()const				{ return m_normalsMode; }
		inline SmoothGroupArray &		GetSmoothGroups			()					{ return m_smoothGroups; }
		inline const SmoothGroupArray &	GetSmoothGroups			()const				{ return m_smoothGroups; }
		inline const SmoothingGroup &	GetSmoothGroup			( size_t i)const	{ CASTOR_ASSERT( i < m_smoothGroups.size());return m_smoothGroups[i]; }
		inline SmoothingGroup &			GetSmoothGroup			( size_t i)			{ CASTOR_ASSERT( i < m_smoothGroups.size());return m_smoothGroups[i]; }
		//@}
	};
	//! The submesh renderer
	/*!
	Initialises the vertex, normals, textures buffers of a submesh, draws it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API SubmeshRenderer : public Renderer<Submesh, SubmeshRenderer>
	{
	protected:
		VertexInfosBufferPtr		m_triangles;			//!< Pointer over geometry triangles vertex buffer
		VertexInfosBufferPtr		m_lines;				//!< Pointer over geometry lines vertex buffer
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
		inline VertexInfosBufferPtr			GetTriangles			()const { return m_triangles; }
		inline VertexInfosBufferPtr			GetLines				()const { return m_lines; }
		//@}
	};
}

#endif
