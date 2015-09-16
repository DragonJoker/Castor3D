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
#ifndef ___C3D_BINARY_PARSER_H___
#define ___C3D_BINARY_PARSER_H___

#include "ChunkParser.hpp"
#include "ChunkFiller.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	template< class TParsed >
	class C3D_API BinaryParser
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
		BinaryParser( Castor::Path const & p_path )
			:	m_path( p_path )
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
		virtual bool Fill( TParsed const & p_obj, Castor::BinaryFile & p_file )const
		{
			BinaryChunk l_chunk;
			bool l_return = Fill( p_obj, l_chunk );

			if ( l_return )
			{
				l_return = l_chunk.Write( p_file );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un fichier
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		virtual bool Parse( TParsed & p_obj, Castor::BinaryFile & p_file )const
		{
			BinaryChunk l_chunk;
			bool l_return = l_chunk.Read( p_file );

			if ( l_return )
			{
				l_return = Parse( p_obj, l_chunk );
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
		virtual bool Fill( TParsed const & p_obj, BinaryChunk & p_chunk )const = 0;
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		virtual bool Parse( TParsed & p_obj, BinaryChunk & p_chunk )const = 0;

	protected:
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_pValues	Receives the parsed values
		 *\param[out]	p_uiCount	The values count
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_pValues	Reçoit les valeurs
		 *\param[out]	p_uiCount	Le compte des valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		bool DoParseChunk( T * p_pValues, uint32_t p_uiCount, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >()( p_pValues, p_uiCount, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_value	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_value	Reçoit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Count >
		bool DoParseChunk( T( & p_value )[Count], BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >()( p_value, Count, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		bool DoParseChunk( T & p_value, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >()( p_value, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The values
		 *\param[in]	p_uiCount		The values count
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		Les valeurs
		 *\param[in]	p_uiCount		Le nombre de valeurs
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		bool DoFillChunk( T const * p_pValues, uint32_t p_uiCount, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )const
		{
			return ChunkFiller< T >()( p_pValues, p_uiCount, p_eChunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The values
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		Les valeurs
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, uint32_t Count >
		bool DoFillChunk( T const( & p_value )[Count], eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )const
		{
			return ChunkFiller< T >()( p_value, Count, p_eChunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value		The value
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value		La valeur
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		bool DoFillChunk( T const & p_value, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )const
		{
			return ChunkFiller< T >()( p_value, p_eChunkType, p_chunk );
		}

		//!\~english The current folder path	\~french Le chemin d'accès au dossiercourant
		Castor::Path m_path;
	};
}

#pragma warning( pop )

#endif
