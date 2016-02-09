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
#ifndef ___C3D_GEOMETRY_BUFFERS_H___
#define ___C3D_GEOMETRY_BUFFERS_H___

#include "Castor3DPrerequisites.hpp"

#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <MatrixBuffer.hpp>

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
		 *\param[in]	p_layout	The program layout.
		 *\param[in]	p_submesh	The submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_topology	La topologie des tampons.
		 *\param[in]	p_layout	Le layout fourni par le programme.
		 *\param[in]	p_submesh	Le sous-maillage.
		 */
		C3D_API GeometryBuffers( eTOPOLOGY p_topology, ProgramInputLayout const & p_layout, VertexBuffer * p_vtx, IndexBuffer * p_idx, VertexBuffer * p_bones, MatrixBuffer * p_inst );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~GeometryBuffers();
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_uiSize	Specifies the number of elements to be rendered
		 *\param[in]	p_index		Specifies the starting index in the enabled arrays
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_uiSize	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index	Spécifie l'indice du premier vertice
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Draw( ShaderProgram const & p_program, uint32_t p_uiSize, uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_uiSize	Specifies the number of elements to be rendered
		 *\param[in]	p_index		Specifies the starting index in the enabled arrays
		 *\param[in]	p_count		The instances count
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_uiSize	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index		Spécifie l'indice du premier vertice
		 *\param[in]	p_count		Le nombre d'instances à dessiner
		 *\return		\p true si tout s'est bien passé
		 */
		/**
		 *\~english
		 *\return		The buffers topology.
		 *\~french
		 *\return		La topologie des tampons.
		 */
		C3D_API virtual bool DrawInstanced( ShaderProgram const & p_program, uint32_t p_uiSize, uint32_t p_index, uint32_t p_count ) = 0;
		/**
		 *\~english
		 *\return		The program layout.
		 *\~french
		 *\return		Le layout du programme.
		 */
		inline ProgramInputLayout const & GetLayout()const
		{
			return m_layout;
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

	protected:
		//!\~english The vertex buffer.	\~french Le tampon de sommets.
		VertexBuffer * m_vertexBuffer = nullptr;
		//!\~english The index buffer.	\~french Le tampon d'indices.
		IndexBuffer * m_indexBuffer = nullptr;
		//!\~english The bones buffer.	\~french Le tampon de bones.
		VertexBuffer * m_bonesBuffer = nullptr;
		//!\~english The matrix buffer.	\~french Le tampon de matrices.
		MatrixBuffer * m_matrixBuffer = nullptr;
		//!\~english The shader program.	\~french Le programme de rendu.
		ProgramInputLayout const & m_layout;
		//!\~english The buffers topology.	\~french La topologie des tampons.
		eTOPOLOGY m_topology;
	};
}

#endif
