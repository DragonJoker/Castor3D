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
	class GlGeometryBuffers;

	class GlGeometryBuffersImpl
		: public Castor::OwnedBy< GlGeometryBuffers >
	{
	public:
		GlGeometryBuffersImpl( GlGeometryBuffers & p_buffers );
		virtual ~GlGeometryBuffersImpl();

		virtual bool Bind() = 0;
		virtual void Unbind() = 0;
	};

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
		, public Castor::NonCopyable
		, public Holder
	{
	public:
		GlGeometryBuffers( OpenGl & p_gl, Castor3D::VertexBufferUPtr p_pVertexBuffer, Castor3D::IndexBufferUPtr p_pIndexBuffer, Castor3D::MatrixBufferUPtr p_pMatrixBuffer );
		virtual ~GlGeometryBuffers();

		virtual bool Draw( Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index );
		virtual bool DrawInstanced( Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index, uint32_t p_count );
		virtual bool Bind();
		virtual void Unbind();

	private:
		virtual bool DoCreate();
		virtual void DoDestroy();
		virtual bool DoInitialise();
		virtual void DoCleanup();

	private:
		std::unique_ptr< GlGeometryBuffersImpl > m_impl;
	};
}

#endif
