/* See LICENSE file in root folder */
#ifndef ___TRS_GEOMETRY_BUFFERS_H___
#define ___TRS_GEOMETRY_BUFFERS_H___

#include "Shader/TestShaderProgram.hpp"

#include <Mesh/Buffer/GeometryBuffers.hpp>

namespace TestRender
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
	class TestGeometryBuffers
		: public castor3d::GeometryBuffers
	{
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
		TestGeometryBuffers( castor3d::Topology p_topology, castor3d::ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TestGeometryBuffers();
		/**
		 *\copydoc		castor3d::GeometryBuffers::Draw
		 */
		virtual bool draw( uint32_t p_size, uint32_t p_index )const override;
		/**
		 *\copydoc		castor3d::GeometryBuffers::DrawInstanced
		 */
		virtual bool drawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const override;

	private:
		/**
		 *\copydoc		castor3d::GeometryBuffers::doInitialise
		 */
		virtual bool doInitialise()override;
		/**
		 *\copydoc		castor3d::GeometryBuffers::doCleanup
		 */
		virtual void doCleanup()override;
		/**
		 *\copydoc		castor3d::GeometryBuffers::doCleanup
		 */
		virtual void doSetTopology( castor3d::Topology p_value )override;
	};
}

#endif
