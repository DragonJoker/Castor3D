/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GL_GEOMETRY_BUFFERS_H___
#define ___GL_GEOMETRY_BUFFERS_H___

#include "GlBufferBase.hpp"
#include "GlShaderProgram.hpp"

#include <GeometryBuffers.hpp>

namespace GlRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		18/11/2012
	\~english
	\brief		Geometry buffers holder
	\remark		Allows implementations to use API specific optimisations (like OpenGL Vertex array objects)
	\~french
	\brief		Conteneur de buffers de géométries
	\remark		Permet aux implémentations d'utiliser les optimisations spécifiques aux API (comme les Vertex arrays objects OpenGL)
	*/
	class GlGeometryBuffers
		: public Castor3D::GeometryBuffers
		, public Bindable <
				 std::function< bool( int, uint32_t * ) >,
				 std::function< bool( int, uint32_t const * ) >,
				 std::function< bool( uint32_t ) >
				 >
	{
		using ObjectType = Bindable <
						   std::function< bool( int, uint32_t * ) >,
						   std::function< bool( int, uint32_t const * ) >,
						   std::function< bool( uint32_t ) >
						   >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_topology	The buffers topology.
		 *\param[in]	p_program	The shader program.
		 *\param[in]	p_vtx		The vertex buffer.
		 *\param[in]	p_idx		The index buffer.
		 *\param[in]	p_bones		The bones data buffer.
		 *\param[in]	p_inst		The instances matrices buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_topology	La topologie des tampons.
		 *\param[in]	p_program	Le programme shader.
		 *\param[in]	p_vtx		Le tampon de sommets.
		 *\param[in]	p_idx		Le tampon d'indices.
		 *\param[in]	p_bones		Le tampon de données de bones.
		 *\param[in]	p_inst		Le tampon de matrices d'instances.
		 */
		GlGeometryBuffers( OpenGl & p_gl, Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgram const & p_program, Castor3D::VertexBuffer * p_vtx, Castor3D::IndexBuffer * p_idx, Castor3D::VertexBuffer * p_bones, Castor3D::VertexBuffer * p_inst );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GlGeometryBuffers();
		/**
		 *\copydoc		Castor3D::GeometryBuffers::Draw
		 */
		virtual bool Draw( uint32_t p_uiSize, uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::GeometryBuffers::DrawInstanced
		 */
		virtual bool DrawInstanced( uint32_t p_uiSize, uint32_t p_index, uint32_t p_count )const;

	private:
		Castor3D::BufferDeclaration::const_iterator DoFindElement( Castor3D::BufferDeclaration const & p_declaration, Castor3D::BufferElementDeclaration const & p_element )const;
		GlAttributeBaseSPtr DoCreateAttribute( Castor3D::BufferElementDeclaration const & p_element, uint32_t p_offset, Castor3D::BufferDeclaration const & p_declaration );
		bool DoCreateAttributes( Castor3D::ProgramInputLayout const & p_layout, Castor3D::BufferDeclaration const & p_declaration, GlAttributePtrArray & p_attributes );
		void DoBindAttributes( GlAttributePtrArray const & p_attributes )const;

	private:
		//! The shader program.
		Castor3D::ShaderProgram const & m_program;
		//! The vertex attributes.
		GlAttributePtrArray m_vertexAttributes;
		//! The instantiation matrix attribute.
		GlAttributePtrArray m_matrixAttributes;
		//! The bones data attributes.
		GlAttributePtrArray m_bonesAttributes;
	};
}

#endif
