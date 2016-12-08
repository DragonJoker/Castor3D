/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GL_GEOMETRY_BUFFERS_H___
#define ___GL_GEOMETRY_BUFFERS_H___

#include "Buffer/GlBufferBase.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Mesh/Buffer/GeometryBuffers.hpp>

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
		, public Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >
	{
		using ObjectType = Bindable< std::function< void( int, uint32_t * ) >
			, std::function< void( int, uint32_t const * ) >
			, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_topology	The buffers topology.
		 *\param[in]	p_program	The shader program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_topology	La topologie des tampons.
		 *\param[in]	p_program	Le programme shader.
		 */
		GlGeometryBuffers( OpenGl & p_gl, Castor3D::Topology p_topology, Castor3D::ShaderProgram const & p_program );
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
		virtual bool Draw( uint32_t p_size, uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::GeometryBuffers::DrawInstanced
		 */
		virtual bool DrawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const;

	private:
		/**
		 *\copydoc		Castor3D::GeometryBuffers::DoInitialise
		 */
		virtual bool DoInitialise();
		/**
		 *\copydoc		Castor3D::GeometryBuffers::DoCleanup
		 */
		virtual void DoCleanup();

		Castor3D::BufferDeclaration::const_iterator DoFindElement( Castor3D::BufferDeclaration const & p_declaration, Castor3D::BufferElementDeclaration const & p_element )const;
		GlAttributeBaseSPtr DoCreateAttribute( Castor3D::BufferElementDeclaration const & p_element, uint32_t p_offset, uint32_t p_divisor, Castor3D::BufferDeclaration const & p_declaration );
		bool DoCreateAttributes( Castor3D::ProgramInputLayout const & p_layout, Castor3D::BufferDeclaration const & p_declaration, GlAttributePtrArray & p_attributes );
		void DoBindAttributes( GlAttributePtrArray const & p_attributes )const;

	private:
		//! The shader program.
		Castor3D::ShaderProgram const & m_program;
		//! The vertex, animation, ... attributes.
		GlAttributePtrArray m_attributes;
		//! The topology type.
		GlTopology m_glTopology;
	};
}

#endif
