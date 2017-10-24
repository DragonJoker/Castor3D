/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_GEOMETRY_BUFFERS_H___
#define ___C3DGLES3_GEOMETRY_BUFFERS_H___

#include "Buffer/GlES3BufferBase.hpp"
#include "Shader/GlES3ShaderProgram.hpp"

#include <Mesh/Buffer/GeometryBuffers.hpp>

namespace GlES3Render
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
	class GlES3GeometryBuffers
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
		GlES3GeometryBuffers( OpenGlES3 & p_gl, Castor3D::Topology p_topology, Castor3D::ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GlES3GeometryBuffers();
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
		GlES3AttributeBaseSPtr DoCreateAttribute( Castor3D::BufferElementDeclaration const & p_element, uint32_t p_offset, uint32_t p_divisor, Castor3D::BufferDeclaration const & p_declaration );
		bool DoCreateAttributes( Castor3D::ProgramInputLayout const & p_layout, Castor3D::BufferDeclaration const & p_declaration, GlES3AttributePtrArray & p_attributes );
		void DoBindAttributes( GlES3AttributePtrArray const & p_attributes )const;

	private:
		//! The shader program.
		Castor3D::ShaderProgram const & m_program;
		//! The vertex, animation, ... attributes.
		GlES3AttributePtrArray m_attributes;
		//! The topology type.
		GlES3Topology m_glTopology;
	};
}

#endif
