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
#ifndef ___C3D_GEOMETRY_BUFFERS_H___
#define ___C3D_GEOMETRY_BUFFERS_H___

#include "Castor3DPrerequisites.hpp"

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		18/11/2012
	\~english
	\brief		Geometry buffers holder
	\remark		Allows implementations to use API specific optimisations (like OpenGL Vertex array objects)
				<br />Acquires it's buffers responsibility when constructed
	\~french
	\brief		Conteneur de buffers de géométries
	\remark		Permet aux implémentations d'utiliser les optimisations spécifiques aux API (comme les Vertex arrays objects OpenGL)
				<br />Acquiert la responsabilité de ses buffers à la création
	*/
	class GeometryBuffers
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
		C3D_API GeometryBuffers( eTOPOLOGY p_topology, ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~GeometryBuffers();
		/**
		 *\~english
		 *\brief		Creates the VAO.
		 *\param[in]	p_vtx	The vertex buffer.
		 *\param[in]	p_anm	The animation vertex buffer.
		 *\param[in]	p_idx	The index buffer.
		 *\param[in]	p_bones	The bones data buffer.
		 *\param[in]	p_inst	The instances matrices buffer.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Crée le VAO.
		 *\param[in]	p_vtx	Le tampon de sommets.
		 *\param[in]	p_anm	Le tampon de sommets d'animation.
		 *\param[in]	p_idx	Le tampon d'indices.
		 *\param[in]	p_bones	Le tampon de données de bones.
		 *\param[in]	p_inst	Le tampon de matrices d'instances.
		 *\return		\p true si ça s'est bien passé.
		 */
		C3D_API bool Initialise( VertexBufferSPtr p_vtx, VertexBufferSPtr p_anm, IndexBufferSPtr p_idx, VertexBufferSPtr p_bones, VertexBufferSPtr p_inst );
		/**
		 *\~english
		 *\brief		Destroys the VAO.
		 *\~french
		 *\brief		Détruit le VAO.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_size	Specifies the number of elements to be rendered
		 *\param[in]	p_index	Specifies the starting index in the enabled arrays
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_size	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index	Spécifie l'indice du premier vertice
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Draw( uint32_t p_size, uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_size	Specifies the number of elements to be rendered
		 *\param[in]	p_index	Specifies the starting index in the enabled arrays
		 *\param[in]	p_count	The instances count
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_size	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index	Spécifie l'indice du premier vertice
		 *\param[in]	p_count	Le nombre d'instances à dessiner
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DrawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const = 0;
		/**
		 *\~english
		 *\return		The program.
		 *\~french
		 *\return		Le programme.
		 */
		inline ShaderProgram const & GetProgram()const
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The buffers topology.
		 *\~french
		 *\return		La topologie des tampons.
		 */
		inline eTOPOLOGY GetTopology()const
		{
			return m_topology;
		}
		/**
		 *\~english
		 *\brief		Sets the buffers topology.
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit la topologie des tampons.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetTopology( eTOPOLOGY p_value )
		{
			m_topology = p_value;
		}

	private:
		/**
		 *\~english
		 *\brief		Creates the VAO.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Crée le VAO.
		 *\return		\p true si ça s'est bien passé.
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Destroys the VAO.
		 *\~french
		 *\brief		Détruit le VAO.
		 */
		C3D_API virtual void DoCleanup() = 0;

	protected:
		//!\~english	The vertex buffer.
		//!\\~french	Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer{ nullptr };
		//!\~english	The animation vertex buffer.
		//!\\~french	Le tampon de sommets d'animation.
		VertexBufferSPtr m_animationBuffer{ nullptr };
		//!\~english	The index buffer.
		//!\\~french	Le tampon d'indices.
		IndexBufferSPtr m_indexBuffer{ nullptr };
		//!\~english	The bones buffer.
		//!\\~french	Le tampon de bones.
		VertexBufferSPtr m_bonesBuffer{ nullptr };
		//!\~english	The matrix buffer.
		//!\\~french	Le tampon de matrices.
		VertexBufferSPtr m_matrixBuffer{ nullptr };
		//!\~english	The shader program.
		//!\\~french	Le programme de rendu.
		ShaderProgram const & m_program;
		//!\~english	The buffers topology.
		//!\\~french	La topologie des tampons.
		eTOPOLOGY m_topology;
	};
}

#endif
