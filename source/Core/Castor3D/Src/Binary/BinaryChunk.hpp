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
#ifndef ___C3D_BINARY_CHUNK_H___
#define ___C3D_BINARY_CHUNK_H___

#include "Castor3DPrerequisites.hpp"

#include <cstring>

namespace Castor3D
{
	//!\~english	Macro to make a version number.
	//!\~french		Macro pour créer un numéro de version.
#	define	MAKE_CMSH_VERSION( major, minor, revision )\
	((uint32_t( major ) << 24)\
	| (uint32_t( minor ) << 16)\
	| (uint32_t( revision ) << 0))
	//!\~english	The current format version number.
	//!\~french		La version actuelle du format.
	uint32_t const CMSH_VERSION = MAKE_CMSH_VERSION( 0x01, 0x00, 0x0000 );
	//!\~english	A define to ease the declaration of a chunk id.
	//!\~french		Un define pour faciliter la déclaration d'un id de chunk.
#	define MAKE_CHUNK_ID( a, b, c, d, e, f, g, h )\
	((uint64_t( a ) << 56)\
	| (uint64_t( b ) << 48)\
	| (uint64_t( c ) << 40)\
	| (uint64_t( d ) << 32)\
	| (uint64_t( e ) << 24)\
	| (uint64_t( f ) << 16)\
	| (uint64_t( g ) << 8)\
	| (uint64_t( h ) << 0))
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk types enumeration
	\~french
	\brief		Enumération des types de chunks de données binaires
	*/
	typedef enum eCHUNK_TYPE
		: uint64_t
	{
		eCHUNK_TYPE_UNKNOWN = MAKE_CHUNK_ID( ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ),
		eCHUNK_TYPE_CMSH_FILE = MAKE_CHUNK_ID( 'C', 'M', 'S', 'H', 'F', 'I', 'L', 'E' ),
		eCHUNK_TYPE_CMSH_HEADER = MAKE_CHUNK_ID( 'C', 'M', 'S', 'H', 'H', 'D', 'E', 'R' ),
		eCHUNK_TYPE_CMSH_VERSION = MAKE_CHUNK_ID( 'C', 'M', 'S', 'H', 'V', 'R', 'S', 'N' ),
		eCHUNK_TYPE_NAME = MAKE_CHUNK_ID( 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' ' ),
		eCHUNK_TYPE_MESH = MAKE_CHUNK_ID( 'M', 'E', 'S', 'H', ' ', ' ', ' ', ' ' ),
		eCHUNK_TYPE_SKELETON = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'E', 'T', 'O', 'N' ),
		eCHUNK_TYPE_SKELETON_GLOBAL_INVERSE = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'G', 'I', 'M', 'X' ),
		eCHUNK_TYPE_SKELETON_BONE = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'B', 'O', 'N', 'E' ),
		eCHUNK_TYPE_BONE_OFFSET_MATRIX = MAKE_CHUNK_ID( 'B', 'O', 'N', 'E', 'O', 'M', 'T', 'X' ),
		eCHUNK_TYPE_BONE_FINAL_TRANSFORM = MAKE_CHUNK_ID( 'B', 'O', 'N', 'E', 'F', 'L', 'T', 'X' ),
		eCHUNK_TYPE_SUBMESH = MAKE_CHUNK_ID( 'S', 'U', 'B', 'M', 'E', 'S', 'H', ' ' ),
		eCHUNK_TYPE_SUBMESH_TOPOLOGY = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'T', 'O', 'P', 'O' ),
		eCHUNK_TYPE_SUBMESH_VERTEX_COUNT = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'V', 'X', 'C', 'T' ),
		eCHUNK_TYPE_SUBMESH_VERTEX = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'V', 'R', 'T', 'X' ),
		eCHUNK_TYPE_SUBMESH_BONE_COUNT = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'B', 'N', 'C', 'T' ),
		eCHUNK_TYPE_SUBMESH_BONES = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'B', 'O', 'N', 'E' ),
		eCHUNK_TYPE_SUBMESH_MATRIX_COUNT = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'M', 'X', 'C', 'T' ),
		eCHUNK_TYPE_SUBMESH_MATRIX = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'M', 'T', 'R', 'X' ),
		eCHUNK_TYPE_SUBMESH_FACE_COUNT = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'F', 'C', 'C', 'T' ),
		eCHUNK_TYPE_SUBMESH_FACES = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'F', 'A', 'C', 'E' ),
		eCHUNK_TYPE_ANIMABLE = MAKE_CHUNK_ID( 'A', 'N', 'I', 'M', 'A', 'B', 'L', 'E' ),
		eCHUNK_TYPE_ANIMATION = MAKE_CHUNK_ID( 'A', 'N', 'M', 'A', 'T', 'I', 'O', 'N' ),
		eCHUNK_TYPE_ANIM_LENGTH = MAKE_CHUNK_ID( 'A', 'N', 'M', 'L', 'E', 'N', 'G', 'T' ),
		eCHUNK_TYPE_SKELETON_ANIMATION = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'A', 'N', 'I', 'M' ),
		eCHUNK_TYPE_ANIM_INTERPOLATOR = MAKE_CHUNK_ID( 'A', 'N', 'M', 'I', 'N', 'T', 'E', 'R' ),
		eCHUNK_TYPE_ANIMATION_OBJECT = MAKE_CHUNK_ID( 'A', 'N', 'I', 'M', 'O', 'B', 'J', 'T' ),
		eCHUNK_TYPE_SKELETON_ANIMATION_NODE = MAKE_CHUNK_ID( 'A', 'N', 'S', 'K', 'N', 'O', 'D', 'E' ),
		eCHUNK_TYPE_SKELETON_ANIMATION_BONE = MAKE_CHUNK_ID( 'A', 'N', 'S', 'K', 'B', 'O', 'N', 'E' ),
		eCHUNK_TYPE_MOVING_TRANSFORM = MAKE_CHUNK_ID( 'M', 'V', 'N', 'G', 'T', 'S', 'F', 'M' ),
		eCHUNK_TYPE_KEYFRAME = MAKE_CHUNK_ID( 'K', 'E', 'Y', 'F', 'R', 'A', 'M', 'E' ),
		eCHUNK_TYPE_KEYFRAME_TIME = MAKE_CHUNK_ID( 'K', 'F', 'R', 'M', 'T', 'I', 'M', 'E' ),
		eCHUNK_TYPE_KEYFRAME_SCALE = MAKE_CHUNK_ID( 'K', 'F', 'R', 'M', 'S', 'C', 'L', 'E' ),
		eCHUNK_TYPE_KEYFRAME_TRANSLATE = MAKE_CHUNK_ID( 'K', 'F', 'R', 'M', 'T', 'S', 'T', 'E' ),
		eCHUNK_TYPE_KEYFRAME_ROTATE = MAKE_CHUNK_ID( 'K', 'F', 'R', 'M', 'R', 'T', 'T', 'E' ),
	}	eCHUNK_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	*/
	template< class Type >
	struct ChunkTyper;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk base class
	\~french
	\brief		Classe de base d'un chunk de données binaires
	*/
	class BinaryChunk
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API BinaryChunk();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type		The chunk type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type du chunk
		 */
		C3D_API BinaryChunk( eCHUNK_TYPE p_type );
		/**
		 *\~english
		 *\brief		Computes the final data buffer from each one added until this call
		 *\~french
		 *\brief		Crée le tampon final à partir de tout ce qui a été ajouté jusqu'à cet appel
		 */
		C3D_API void Finalise();
		/**
		 *\~english
		 *\brief		Adds data to the chunk
		 *\param[in]	p_data	The data buffer
		 *\param[in]	p_size	The buffer size
		 *\~french
		 *\brief		Ajoute des données au chunk
		 *\param[in]	p_data	Le tampon de données
		 *\param[in]	p_size	La taille du tampon
		 */
		C3D_API void Add( uint8_t * p_data, uint32_t p_size );
		/**
		 *\~english
		 *\brief		Retrieves data from the chunk
		 *\param[in]	p_data	The data buffer to fill
		 *\param[in]	p_size	The buffer size
		 *\~french
		 *\brief		Récupère des données du chunk
		 *\param[in]	p_data	Le tampon de données à remplir
		 *\param[in]	p_size	La taille du tampon
		 */
		C3D_API void Get( uint8_t * p_data, uint32_t p_size );
		/**
		 *\~english
		 *\brief		Checks that the remaining place can hold the given size
		 *\param[in]	p_size	The size
		 *\~french
		 *\brief		Vérifie que la place restante peut contenir la taille donnée
		 *\param[in]	p_size	La taille
		 */
		C3D_API bool CheckAvailable( uint32_t p_size = 0 )const;
		/**
		 *\~english
		 *\brief		Retrieves the remaining place
		 *\return		The value
		 *\~french
		 *\brief		Récupère la place restante
		 *\return		La valeur
		 */
		C3D_API uint32_t GetRemaining()const;
		/**
		 *\~english
		 *\brief		Retrieves a subchunk
		 *\param[out]	p_subchunk	Receives the subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un sous chunk
		 *\param[out]	p_subchunk	Reçoit le sous chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool GetSubChunk( BinaryChunk & p_subchunk );
		/**
		 *\~english
		 *\brief		Writes a subchunk into a chunk
		 *\param[in]	p_subchunk	The subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit un subchunk dans un chunk
		 *\param[in]	p_subchunk	Le subchunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool AddSubChunk( BinaryChunk const & p_subchunk );
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_file	The file
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un fichier
		 *\param[in]	p_file	Le fichier
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool Write( Castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[in]	p_file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un fichier
		 *\param[in]	p_file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool Read( Castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		Retrieves the remaining data
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tampon restant
		 *\return		La valeur
		 */
		inline uint8_t const * GetRemainingData()const
		{
			return &m_data[m_index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de chunk
		 *\return		La valeur
		 */
		inline eCHUNK_TYPE GetChunkType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille des données du chunk
		 *\return		La valeur
		 */
		inline uint32_t GetDataSize()const
		{
			return uint32_t( m_data.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données du chunk
		 *\return		La valeur
		 */
		inline uint8_t const * GetData()const
		{
			return m_data.data();
		}
		/**
		 *\~english
		 *\brief		Sets the chunk's data
		 *\param[in]	p_data	The data buffer
		 *\param[in]	p_size	The buffer size
		 *\~french
		 *\brief		Définit les données du chunk
		 *\param[in]	p_data	Le tampon de données
		 *\param[in]	p_size	La taille du tampon
		 */
		inline void SetData( uint8_t const * p_begin, uint8_t const * p_end )
		{
			m_data.assign( p_begin, p_end );
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data size
		 *\~french
		 *\brief		Récupère la taille des données du chunk
		 */
		void EndParse()
		{
			m_index = uint32_t( m_data.size() );
		}

	private:
		template< typename T >
		inline bool DoRead( T * p_values, uint32_t p_count )
		{
			uint32_t l_size = p_count * uint32_t( sizeof( T ) );
			bool l_return = m_index + l_size < m_data.size();

			if ( l_return )
			{
				std::memcpy( p_values, &m_data[m_index], l_size );
				m_index += l_size;
			}

			return l_return;
		}

	private:
		//!\~english The chunk type	\~french Le type du chunk
		eCHUNK_TYPE m_type;
		//!\~english The chunk data	\~french Les données du chunk
		Castor::ByteArray m_data;
		//!\~english The current index in the chunk data	\~french L'index courant dans les données du chunk
		uint32_t m_index;
		//!\~english The chunk data	\~french Les données du chunk
		std::list< Castor::ByteArray > m_addedData;
	};
}

#endif
