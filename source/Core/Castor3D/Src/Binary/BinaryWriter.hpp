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
#ifndef ___C3D_BINARY_WRITER_H___
#define ___C3D_BINARY_WRITER_H___

#include "ChunkWriter.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk writer/reader base class
	\~french
	\brief		Classe de base de lecture/écriture d'un chunk de données binaires
	*/
	template< class TWritten >
	class BinaryWriter
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_path	The current folder path
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_path	Le chemin d'accès au dossier courant
		 */
		inline BinaryWriter( Castor::Path const & p_path )
			: m_path{ p_path }
		{
		}
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_obj	The object to write
		 *\param[in]	p_file	The file
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un fichier
		 *\param[in]	p_obj	L'objet à écrire
		 *\param[in]	p_file	Le fichier
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool Write( TWritten const & p_obj, Castor::BinaryFile & p_file )const
		{
			BinaryChunk l_chunk;
			bool l_return = DoWrite( p_obj, l_chunk );

			if ( l_return )
			{
				l_return = l_chunk.Write( p_file );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_obj	The object to write
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un chunk
		 *\param[in]	p_obj	L'objet à écrire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool Write( TWritten const & p_obj, BinaryChunk & p_chunk )const
		{
			return DoWrite( p_obj, p_chunk );
		}

	protected:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_values	The values
		 *\param[in]	p_count		The values count
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_values	Les valeurs
		 *\param[in]	p_count		Le nombre de valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		inline bool DoWriteChunk( T const * p_values, size_t p_count, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return DoWriteChunk( p_values, p_values + p_count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The values
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		Les valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, size_t Count >
		inline bool DoWriteChunk( T const( & p_value )[Count], eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return DoWriteChunk( p_value, Count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The values
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		Les valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, size_t Count >
		inline bool DoWriteChunk( std::array< T, Count > const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return DoWriteChunk( p_value.data(), Count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The values
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		Les valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		inline bool DoWriteChunk( std::vector< T > const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return DoWriteChunk( p_value.data(), p_value.size(), p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_values	The values
		 *\param[in]	p_count		The values count
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_values	Les valeurs
		 *\param[in]	p_count		Le nombre de valeurs
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		inline bool DoWriteChunk( T const * p_begin, T const * p_end, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_begin, p_end, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_chunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_chunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		inline bool DoWriteChunk( T const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_value, p_chunkType, p_chunk );
		}

	private:
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_obj	The object to write
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un chunk
		 *\param[in]	p_obj	L'objet à écrire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API virtual bool DoWrite( TWritten const & p_obj, BinaryChunk & p_chunk )const = 0;

	protected:
		//!\~english	The current folder path.
		//!\~french		Le chemin d'accès au dossier courant.
		Castor::Path m_path;
	};
}

#endif
