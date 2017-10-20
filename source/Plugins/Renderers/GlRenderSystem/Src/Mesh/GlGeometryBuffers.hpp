/*
See LICENSE file in root folder
*/
#ifndef ___GL_GEOMETRY_BUFFERS_H___
#define ___GL_GEOMETRY_BUFFERS_H___

#include "Buffer/GlBuffer.hpp"
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
		: public castor3d::GeometryBuffers
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
		 *\param[in]	topology	The buffers topology.
		 *\param[in]	program		The shader program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	topology	La topologie des tampons.
		 *\param[in]	program		Le programme shader.
		 */
		GlGeometryBuffers( OpenGl & gl
			, castor3d::Topology topology
			, castor3d::ShaderProgram const & program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GlGeometryBuffers();
		/**
		 *\copydoc		castor3d::GeometryBuffers::Draw
		 */
		virtual bool draw( uint32_t size
			, uint32_t index )const;
		/**
		 *\copydoc		castor3d::GeometryBuffers::DrawInstanced
		 */
		virtual bool drawInstanced( uint32_t size
			, uint32_t index
			, uint32_t count )const;

	private:
		/**
		 *\copydoc		castor3d::GeometryBuffers::doInitialise
		 */
		virtual bool doInitialise();
		/**
		 *\copydoc		castor3d::GeometryBuffers::doCleanup
		 */
		virtual void doCleanup();

		castor3d::BufferDeclaration::const_iterator doFindElement( castor3d::BufferDeclaration const & declaration
			, castor3d::BufferElementDeclaration const & element )const;
		GlAttributeBaseSPtr doCreateAttribute( castor3d::BufferElementDeclaration const & element
			, uint32_t offset
			, uint32_t divisor
			, castor3d::BufferDeclaration const & declaration );
		bool doCreateAttributes( castor3d::ProgramInputLayout const & layout
			, castor3d::BufferDeclaration const & declaration
			, uint32_t offset
			, GlAttributePtrArray & attributes );
		void doBindAttributes( GlAttributePtrArray const & attributes )const;
		void doDrawElementsIndirect( uint32_t size
			, uint32_t index
			, uint32_t count )const;
		void doDrawArraysIndirect( uint32_t size
			, uint32_t index
			, uint32_t count )const;

	private:
		//! The shader program.
		castor3d::ShaderProgram const & m_program;
		//! The vertex, animation, ... attributes.
		GlAttributePtrArray m_attributes;
		//! The topology type.
		GlTopology m_glTopology;
		GlBuffer m_glIndirectArraysBuffer;
		GlBuffer m_glIndirectElementsBuffer;
		mutable DrawArraysIndirectCommand m_arraysCommand;
		mutable DrawElementsIndirectCommand m_elementsCommand;
	};
}

#endif
