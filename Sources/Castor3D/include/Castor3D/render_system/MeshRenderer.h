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
#ifndef ___C3D_MeshRenderer___
#define ___C3D_MeshRenderer___

#include "Renderer.h"
#include "Buffer.h"

namespace Castor3D
{
	//! The submesh renderer
	/*!
	Initialises the vertex, normals, textures buffers of a submesh, draws it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API SubmeshRenderer : public Renderer<Submesh>
	{
	protected:
		friend class RenderSystem;
		VertexBuffer					*	m_triangles;			//!< Pointer over geometry triangles vertex buffer
		VertexAttribsBuffer<real>		*	m_trianglesNormals;		//!< Pointer over geometry triangles normals buffer
		VertexAttribsBuffer<real>		*	m_trianglesTangents;	//!< Pointer over geometry triangles tangents buffer
		TextureBuffer					*	m_trianglesTexCoords;	//!< Pointer over geometry triangles texture coords buffer
		VertexBuffer					*	m_lines;				//!< Pointer over geometry lines vertex buffer
		NormalsBuffer					*	m_linesNormals;			//!< Pointer over geometry lines normals buffer
		TextureBuffer					*	m_linesTexCoords;		//!< Pointer over geometry lines texture coords buffer
		/**
		 * Constructor, only RenderSystem can use it
		 */
		SubmeshRenderer( RenderSystem * p_rs)
			:	Renderer<Submesh>( p_rs)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~SubmeshRenderer(){ Cleanup(); }
		/**
		 * Cleans this renderer
		 */
		virtual void Cleanup(){}
		/**
		* Initialises the geometry's buffers
		*/
		virtual void Initialise() = 0;
		/**
		* Initialises the geometry's buffers
		*/
		virtual void Draw( DrawType p_mode, PassPtr p_pass) = 0;
		/**
		 * Shows the submesh vertices (enables its arrays)
		 *@param p_displayMode : [in] The wanted disply mode
		 */
		virtual void ShowVertex( DrawType p_displayMode) = 0;
		/**
		 * Hides the submesh vertices (disables its arrays)
		 */
		virtual void HideVertex() = 0;

	public:
		inline VertexBuffer					*	GetTriangles			()const { return m_triangles; }
		inline VertexAttribsBuffer<real>	*	GetTrianglesNormals		()const { return m_trianglesNormals; }
		inline VertexAttribsBuffer<real>	*	GetTrianglesTangents	()const { return m_trianglesTangents; }
		inline TextureBuffer				*	GetTrianglesTexCoords	()const { return m_trianglesTexCoords; }
		inline VertexBuffer					*	GetLines				()const { return m_lines; }
		inline NormalsBuffer				*	GetLinesNormals			()const { return m_linesNormals; }
		inline TextureBuffer				*	GetLinesTexCoords		()const { return m_linesTexCoords; }
	};
}

#endif
