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
	\brief		Conteneur de buffers de g�om�tries
	\remark		Permet aux impl�mentations d'utiliser les optimisations sp�cifiques aux API (comme les Vertex arrays objects OpenGL)
				<br />Acquiert la responsabilit� de ses buffers � la cr�ation
	*/
	class C3D_API GeometryBuffers
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pVertexBuffer	The vertex buffer
		 *\param[in]	p_pIndexBuffer	The index buffer
		 *\param[in]	p_pMatrixBuffer	The matrix buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pVertexBuffer	Le tampon de sommets
		 *\param[in]	p_pIndexBuffer	Le tampon d'indices
		 *\param[in]	p_pMatrixBuffer	Le tampon de matrices
		 */
		GeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GeometryBuffers();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_eTopology	The wanted topology
		 *\param[in]	p_uiSize	Specifies the number of elements to be rendered
		 *\param[in]	p_uiIndex	Specifies the starting index in the enabled arrays
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la g�om�trie contenue dans les buffers
		 *\param[in]	p_eTopology	La topologie voulue
		 *\param[in]	p_uiSize	Sp�cifie le nombre de vertices � rendre
		 *\param[in]	p_uiIndex	Sp�cifie l'indice du premier vertice
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Draw( eTOPOLOGY p_eTopology, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex ) = 0;
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_eTopology	The wanted topology
		 *\param[in]	p_uiSize	Specifies the number of elements to be rendered
		 *\param[in]	p_uiIndex	Specifies the starting index in the enabled arrays
		 *\param[in]	p_uiCount	The instances count
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la g�om�trie contenue dans les buffers
		 *\param[in]	p_eTopology	La topologie voulue
		 *\param[in]	p_uiSize	Sp�cifie le nombre de vertices � rendre
		 *\param[in]	p_uiIndex	Sp�cifie l'indice du premier vertice
		 *\param[in]	p_uiCount	Le nombre d'instances � dessiner
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool DrawInstanced( eTOPOLOGY p_eTopology, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount ) = 0;
		/**
		 *\~english
		 *\brief		Binds the geometry buffers
		 *\remark		If it fails they will be correctly unbound
		 *\return		\p false if one buffer failed to be bound
		 *\~french
		 *\brief		Active les buffers de la g�om�trie
		 *\remark		Si �a �choue, les buffers seront correctement d�sactiv�s
		 *\return		\p false si l'activation d'un tampon a �chou�
		 */
		virtual bool Bind();
		/**
		 *\~english
		 *\brief		Unbinds the geometry buffers
		 *\~french
		 *\brief		D�sactive les buffers de la g�om�trie
		 */
		virtual void Unbind();
		/**
		 *\~english
		 *\brief		Retrieves the indices buffer
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le tampon d'indices
		 *\return		La valeur
		 */
		inline IndexBuffer & GetIndexBuffer()
		{
			return *m_pIndexBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertices buffer
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le tampon de vertices
		 *\return		La valeur
		 */
		inline VertexBuffer & GetVertexBuffer()
		{
			return *m_pVertexBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the matrix buffer
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le tampon de matrices
		 *\return		La valeur
		 */
		inline MatrixBuffer & GetMatrixBuffer()
		{
			return *m_pMatrixBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves if the geometry buffers has an indices buffer
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le fait que le conteneur poss�de un tampon d'indices
		 *\return		La valeur
		 */
		inline bool HasIndexBuffer()
		{
			return m_bIndexBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves if the geometry buffers has a matrix buffer
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le fait que le conteneur poss�de un tampon de matrices
		 *\return		La valeur
		 */
		inline bool HasMatrixBuffer()
		{
			return m_bMatrixBuffer;
		}

	protected:
		//!\~english The vertex buffer	\~french Le tampon de sommets
		VertexBufferUPtr m_pVertexBuffer;
		//!\~english The index buffer	\~french Le tampon d'indices
		IndexBufferUPtr m_pIndexBuffer;
		//!\~english The matrix buffer	\~french Le tampon de matrices
		MatrixBufferUPtr m_pMatrixBuffer;
		//!\~english Tells the geometry buffers has an index buffer	\~french Dit si le conteneur poss�de un tampon d'indices
		bool m_bIndexBuffer;
		//!\~english Tells the geometry buffers has a matrix buffer	\~french Dit si le conteneur poss�de un tampon de matrices
		bool m_bMatrixBuffer;
	};
}

#endif
