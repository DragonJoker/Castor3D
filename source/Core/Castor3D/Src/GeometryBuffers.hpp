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
		C3D_API GeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~GeometryBuffers();
		/**
		 *\~english
		 *\brief		Creates the buffers on GPU
		 *\return		\p true if OK
		 *\~french
		 *\brief		Crée les tampons au niveau GPU
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Create();
		/**
		 *\~english
		 *\brief		Destroys the buffers on GPU
		 *\~french
		 *\brief		Détruit les tampons au niveau GPU
		 */
		C3D_API void Destroy();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_shader				The shader program
		 *\param[in]	p_vtxType, p_vtxAccess	Vertex buffer access flags
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_shader				Le programme shader
		 *\param[in]	p_vtxType, p_vtxAccess	Indicateurs d'accès pour le tampon de sommets
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature );
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_shader				The shader program
		 *\param[in]	p_vtxType, p_vtxAccess	Vertex buffer access flags
		 *\param[in]	p_idxType, p_idxAccess	Index buffer access flags
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_shader				Le programme shader
		 *\param[in]	p_vtxType, p_vtxAccess	Indicateurs d'accès pour le tampon de sommets
		 *\param[in]	p_idxType, p_idxAccess	Indicateurs d'accès pour le tampon d'indices
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature, eBUFFER_ACCESS_TYPE p_idxType, eBUFFER_ACCESS_NATURE p_idxNature );
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_shader				The shader program
		 *\param[in]	p_vtxType, p_vtxAccess	Vertex buffer access flags
		 *\param[in]	p_idxType, p_idxAccess	Index buffer access flags
		 *\param[in]	p_mtxType, p_mtxAccess	Matrix buffer access flags
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_shader				Le programme shader
		 *\param[in]	p_vtxType, p_vtxAccess	Indicateurs d'accès pour le tampon de sommets
		 *\param[in]	p_idxType, p_idxAccess	Indicateurs d'accès pour le tampon d'indices
		 *\param[in]	p_mtxType, p_mtxAccess	Indicateurs d'accès pour le tampon de matrices
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature, eBUFFER_ACCESS_TYPE p_idxType, eBUFFER_ACCESS_NATURE p_idxNature, eBUFFER_ACCESS_TYPE p_mtxType, eBUFFER_ACCESS_NATURE p_mtxNature );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_eTopology	The wanted topology
		 *\param[in]	p_uiSize	Specifies the number of elements to be rendered
		 *\param[in]	p_index	Specifies the starting index in the enabled arrays
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_eTopology	La topologie voulue
		 *\param[in]	p_uiSize	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index	Spécifie l'indice du premier vertice
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Draw( eTOPOLOGY p_eTopology, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_eTopology	The wanted topology
		 *\param[in]	p_uiSize	Specifies the number of elements to be rendered
		 *\param[in]	p_index	Specifies the starting index in the enabled arrays
		 *\param[in]	p_uiCount	The instances count
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_eTopology	La topologie voulue
		 *\param[in]	p_uiSize	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index	Spécifie l'indice du premier vertice
		 *\param[in]	p_uiCount	Le nombre d'instances à dessiner
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DrawInstanced( eTOPOLOGY p_eTopology, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_index, uint32_t p_uiCount ) = 0;
		/**
		 *\~english
		 *\brief		Binds the geometry buffers
		 *\remark		If it fails they will be correctly unbound
		 *\return		\p false if one buffer failed to be bound
		 *\~french
		 *\brief		Active les buffers de la géométrie
		 *\remark		Si ça échoue, les buffers seront correctement désactivés
		 *\return		\p false si l'activation d'un tampon a échoué
		 */
		C3D_API virtual bool Bind();
		/**
		 *\~english
		 *\brief		Unbinds the geometry buffers
		 *\~french
		 *\brief		Désactive les buffers de la géométrie
		 */
		C3D_API virtual void Unbind();
		/**
		 *\~english
		 *\brief		Retrieves the indices buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tampon d'indices
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
		 *\brief		Récupère le tampon de vertices
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
		 *\brief		Récupère le tampon de matrices
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
		 *\brief		Récupère le fait que le conteneur possède un tampon d'indices
		 *\return		La valeur
		 */
		inline bool HasIndexBuffer()
		{
			return m_pIndexBuffer != nullptr;
		}
		/**
		 *\~english
		 *\brief		Retrieves if the geometry buffers has a matrix buffer
		 *\return		The value
		 *\~french
		 *\brief		Récupère le fait que le conteneur possède un tampon de matrices
		 *\return		La valeur
		 */
		inline bool HasMatrixBuffer()
		{
			return m_pMatrixBuffer != nullptr;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates the GPU related stuff
		 *\return		\p true if OK
		 *\~french
		 *\brief		Crée les objets GPU
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DoCreate() = 0;
		/**
		 *\~english
		 *\brief		Destroys the GPU related stuff
		 *\~french
		 *\brief		Détruit les objets GPU
		 */
		C3D_API virtual void DoDestroy() = 0;
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void DoCleanup() = 0;

	protected:
		//!\~english The vertex buffer	\~french Le tampon de sommets
		VertexBufferUPtr m_pVertexBuffer;
		//!\~english The index buffer	\~french Le tampon d'indices
		IndexBufferUPtr m_pIndexBuffer;
		//!\~english The matrix buffer	\~french Le tampon de matrices
		MatrixBufferUPtr m_pMatrixBuffer;
	};
}

#endif
