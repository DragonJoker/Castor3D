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

#include "../../Prerequisites.h"
#include "../../render_system/Renderable.h"

namespace Castor3D
{
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
		VertexBufferPtr		m_vertex;					//!< Pointer over geometry vertex buffer
		IndexBufferPtr		m_indices[eNbDrawTypes];	//!< Pointer over geometry indices buffer
		ePRIMITIVE_TYPE			m_eLastDrawType;
		eNORMALS_MODE		m_eLastNormalsMode;
		/**
		 * Constructor, only RenderSystem can use it
		 */
		SubmeshRenderer( SceneManager * p_pManager);

	public:
		/**
		 * Destructor
		 */
		virtual ~SubmeshRenderer();
		/**
		 * Cleans the renderer
		 */
		virtual void Cleanup();
		/**
		* Initialises the geometry's buffers
		*/
		virtual void Draw( ePRIMITIVE_TYPE p_eMode, const Pass & p_pass);
		/**
		* Initialises the geometry's buffers
		*/
		virtual void Initialise() = 0;

//		virtual VertexBufferPtr CreateVertexBuffer( BufferDeclarationPtr p_pBufferDeclaration)=0;

	public:
		/**@name Accessors */
		//@{
		inline void	SetVertex( VertexBufferPtr p_vertex)	{ m_vertex = p_vertex; }

		inline VertexBufferPtr	GetVertex		()const { return m_vertex; }
		inline IndexBufferPtr	GetIndices		( ePRIMITIVE_TYPE p_eType)const { CASTOR_ASSERT( p_eType < eNbDrawTypes);return m_indices[p_eType]; }
		//@}

	protected:
		virtual void _drawBuffers( const Pass & p_pass) = 0;
		void _createVbo();
	};
	//! The submesh representation
	/*!
	A submesh holds its buffers (vertex, normas and texture) its smoothgroups and its combobox
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Submesh : public Serialisable, public Textable, public Renderable<Submesh, SubmeshRenderer>
	{
	protected:
		weak_ptr<Material>		m_material;				//!< The material
		String					m_strMatName;			//!< The materialname
		SmoothGroupPtrArray		m_smoothGroups;			//!< The smoothgroups (which hold the faces)
		CubeBox					m_box;					//!< The combo box container
		SphereBox				m_sphere;				//!< The spheric container
		IdPoint3rPtrArray		m_points;				//!< The vertex pointer array
		VertexBufferPtr			m_vertex;				//!< Pointer over geometry vertex
		IndexBufferPtr			m_indices[eNbDrawTypes];//!< Pointer over geometry indices buffer
		eNORMALS_MODE			m_normalsMode;			//!< The current normal's mode

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
		int IsInMyPoints( const Point3r & p_vertex);
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
		IdPoint3rPtr AddPoint( real x, real y, real z);
		/**
		 * Adds a vertex to my list
		 *@param p_v : [in] The vertex to add
		 *@return The vertex
		 */
		IdPoint3rPtr AddPoint( const Point3r & p_v);
		/**
		 * Creates and adds a vertex to my list
		 *@param p_v : [in] The vertex coordinates
		 *@return The created vertex
		 */
		IdPoint3rPtr AddPoint( real * p_v);
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
		 * Creates and adds a face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex index
		 *@param b : [in] The second face's vertex index
		 *@param c : [in] The third face's vertex index
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		FacePtr AddFace( size_t a, size_t b, size_t c, SmoothingGroup & p_pGroup);
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
		void AddSmoothGroup( SmoothingGroupPtr p_group);
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
		void Subdivide( SubdividerPtr p_subdivider, Point3r * p_center=NULL, bool p_bGenerateBuffers=true, bool p_bThreaded=false);
		/**
		 * 
		 */
		void ComputeFacesFromPolygonVertex();
		/**
		 * 
		 */	
		void GetSmoothGroups( SmoothGroupPtrArray & p_array);
		/**
		 * Renders the submesh
		 *@param p_displayMode : [in] The display mode
		 */
		virtual void Render( ePRIMITIVE_TYPE p_displayMode);

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const;
		virtual bool Read( File & p_file) { return false; }
		//@}

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

		/**@name Accessors */
		//@{
		inline void SetMaterial		( MaterialPtr p_mat)					{ m_material = p_mat; }
		inline void	SetVertex		( VertexBufferPtr p_vertex)				{ m_vertex = p_vertex;m_pRenderer->SetVertex( p_vertex); }
		inline void	SetPoints		( const IdPoint3rPtrArray & p_points)	{ m_points = p_points; }
		inline void	SetSmoothGroups	( const SmoothGroupPtrArray & p_groups)	{ m_smoothGroups = p_groups; }
		inline size_t							GetNbSmoothGroups		()const				{ return m_smoothGroups.size(); }
		inline size_t							GetNbPoints				()const				{ return m_points.size(); }
		inline IdPoint3rPtr						operator []				( size_t i)const	{ CASTOR_ASSERT( i < m_points.size());return m_points[i]; }
		inline IdPoint3rPtr						GetPoint				( size_t i)const	{ CASTOR_ASSERT( i < m_points.size());return m_points[i]; }
		inline SmoothingGroupPtr				GetSmoothGroup			( size_t i)const	{ CASTOR_ASSERT( i < m_smoothGroups.size());return m_smoothGroups[i]; }
		inline MaterialPtr						GetMaterial				()const				{ return m_material.expired() ? MaterialPtr() : m_material.lock(); }
		inline eNORMALS_MODE					GetNormalsMode			()const				{ return m_normalsMode; }
		inline const CubeBox				&	GetCubeBox				()const				{ return m_box; }
		inline const SphereBox				&	GetSphere				()const				{ return m_sphere; }
		inline const IdPoint3rPtrArray		&	GetPoints				()const				{ return m_points; }
		inline const SmoothGroupPtrArray	&	GetSmoothGroups			()const				{ return m_smoothGroups; }
		inline CubeBox						&	GetCubeBox				()					{ return m_box; }
		inline SphereBox					&	GetSphere				()					{ return m_sphere; }
		inline IdPoint3rPtrArray			&	GetPoints				()					{ return m_points; }
		inline SmoothGroupPtrArray			&	GetSmoothGroups			()					{ return m_smoothGroups; }
		//@}

	private:
		FacePtr _addFace( const FacePtr p_face, size_t p_sgIndex);
		void _generateVertex();
		void _generateTrianglesIndices();
		void _generateLinesIndices();
		void _generatePointsIndices();
	};
}

#endif
