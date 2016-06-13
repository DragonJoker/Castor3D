/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
		: public Castor3D::GeometryBuffers
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
		TestGeometryBuffers( Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TestGeometryBuffers();
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
	};
}

#endif
