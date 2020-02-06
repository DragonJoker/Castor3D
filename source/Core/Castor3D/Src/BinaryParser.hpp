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
	\brief		Classe de base de lecture/�criture d'un chunk de donn�es binaires
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
		 *\param[in]	p_path	Le chemin d'acc�s au dossier courant
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
		 *\brief		Fonction d'�criture dans un fichier
		 *\param[in]	p_obj	L'objet � �crire
		 *\param[in]	p_file	Le fichier
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		virtual bool Fill( TParsed const & p_obj, Castor::BinaryFile & p_file )const
		{
			BinaryChunk l_chunk;
			bool l_bReturn = Fill( p_obj, l_chunk );

			if ( l_bReturn )
			{
				l_bReturn = l_chunk.Write( p_file );
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture � partir d'un fichier
		 *\param[out]	p_obj	L'objet � lire
		 *\param[in]	p_file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		virtual bool Parse( TParsed & p_obj, Castor::BinaryFile & p_file )const
		{
			BinaryChunk l_chunk;
			bool l_bReturn = l_chunk.Read( p_file );

			if ( l_bReturn )
			{
				l_bReturn = Parse( p_obj, l_chunk );
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_obj	The object to write
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'�criture dans un chunk
		 *\param[in]	p_obj	L'objet � �crire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		virtual bool Fill( TParsed const & p_obj, BinaryChunk & p_chunk )const = 0;
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture � partir d'un chunk
		 *\param[out]	p_obj	L'objet � lire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arriv�e
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
		 *\brief		R�cup�re un tableau de valeurs � partir d'un chunk
		 *\param[out]	p_pValues	Re�oit les valeurs
		 *\param[out]	p_uiCount	Le compte des valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		template< typename T >
		bool DoParseChunk( T * p_pValues, uint32_t p_uiCount, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >()( p_pValues, p_uiCount, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_tValue	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		R�cup�re un tableau de valeurs � partir d'un chunk
		 *\param[out]	p_tValue	Re�oit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		template< typename T, uint32_t Count >
		bool DoParseChunk( T( & p_tValue )[Count], BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >()( p_tValue, Count, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_tValue	Receives the parsed value
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		R�cup�re une valeur � partir d'un chunk
		 *\param[out]	p_tValue	Re�oit la valeur
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		template< typename T >
		bool DoParseChunk( T & p_tValue, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >()( p_tValue, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_tValue		The values
		 *\param[in]	p_uiCount		The values count
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_tValue		Les valeurs
		 *\param[in]	p_uiCount		Le nombre de valeurs
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		template< typename T >
		bool DoFillChunk( T const * p_pValues, uint32_t p_uiCount, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )const
		{
			return ChunkFiller< T >()( p_pValues, p_uiCount, p_eChunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_tValue		The values
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_tValue		Les valeurs
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		template< typename T, uint32_t Count >
		bool DoFillChunk( T const( & p_tValue )[Count], eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )const
		{
			return ChunkFiller< T >()( p_tValue, Count, p_eChunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_tValue		The value
		 *\param[in]	p_eChunkType	The subchunk type
		 *\param[in]	p_chunk			The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_tValue		La valeur
		 *\param[in]	p_eChunkType	Le type du subchunk
		 *\param[in]	p_chunk			Le chunk
		 *\return		\p false si une erreur quelconque est arriv�e
		 */
		template< typename T >
		bool DoFillChunk( T const & p_tValue, eCHUNK_TYPE p_eChunkType, BinaryChunk & p_chunk )const
		{
			return ChunkFiller< T >()( p_tValue, p_eChunkType, p_chunk );
		}

		//!\~english The current folder path	\~french Le chemin d'acc�s au dossiercourant
		Castor::Path m_path;
	};
}

#pragma warning( pop )

#endif
