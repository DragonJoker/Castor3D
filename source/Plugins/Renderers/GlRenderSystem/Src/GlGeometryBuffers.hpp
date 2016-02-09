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

#include <Buffer.hpp>

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
		GlGeometryBuffers( OpenGl & p_gl, Castor3D::eTOPOLOGY p_topology, Castor3D::ProgramInputLayout const & p_layout, Castor3D::VertexBuffer * p_vtx, Castor3D::IndexBuffer * p_idx, Castor3D::VertexBuffer * p_bones, Castor3D::MatrixBuffer * p_inst );
		virtual ~GlGeometryBuffers();

		virtual bool Draw( Castor3D::ShaderProgram const & p_program, uint32_t p_uiSize, uint32_t p_index );
		virtual bool DrawInstanced( Castor3D::ShaderProgram const & p_program, uint32_t p_uiSize, uint32_t p_index, uint32_t p_count );
	};
}

#endif
