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
#ifndef ___C3D_CHUNK_PARSER_H___
#define ___C3D_CHUNK_PARSER_H___

#include "BinaryChunk.hpp"
#include "ChunkData.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk parser
	\~french
	\brief		Analyseur de chunk
	*/
	class ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[out]	p_size		The values size
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[out]	p_size		La taille des valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool Parse( uint8_t * p_values, uint32_t p_size, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( p_size );

			if ( l_return )
			{
				p_chunk.Get( p_values, p_size );
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk parser
	\~french
	\brief		Analyseur de chunk
	*/
	template< typename T >
	class ChunkParser
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[out]	p_count		The values count
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[out]	p_count		Le compte des valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool Parse( T * p_values, uint32_t p_count, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::Parse( reinterpret_cast< uint8_t * >( p_values ), p_count * sizeof( T ), p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< uint32_t Count >
		static inline bool Parse( T( & p_values )[Count], BinaryChunk & p_chunk )
		{
			return ChunkParserBase::Parse( reinterpret_cast< uint8_t * >( p_values ), Count * sizeof( T ), p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool Parse( T & p_value, BinaryChunk & p_chunk )
		{
			return ChunkParserBase::Parse( ChunkData< T >::GetBuffer( p_value ), uint32_t( ChunkData< T >::GetDataSize( p_value ) ), p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::String
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::String
	*/
	template<>
	class ChunkParser< Castor::String >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool Parse( Castor::String & p_value, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( 1 );
			uint32_t l_uiSize = p_chunk.GetRemaining();

			if ( l_return )
			{
				std::vector< char > l_pChar( l_uiSize + 1, 0 );
				l_return = ChunkParserBase::Parse( reinterpret_cast< uint8_t * >( l_pChar.data() ), l_uiSize, p_chunk );

				if ( l_return )
				{
					p_value = Castor::string::string_cast< xchar >( l_pChar.data() );
				}
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for Castor::Path
	\~french
	\brief		Spécialisation de ChunkParser pour Castor::Path
	*/
	template<>
	class ChunkParser< Castor::Path >
		: public ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool Parse( Castor::Path & p_value, BinaryChunk & p_chunk )
		{
			bool l_return = p_chunk.CheckAvailable( 1 );
			uint32_t l_uiSize = p_chunk.GetRemaining();

			if ( l_return )
			{
				std::vector< char > l_pChar( l_uiSize + 1, 0 );
				l_return = ChunkParserBase::Parse( reinterpret_cast< uint8_t * >( l_pChar.data() ), l_uiSize, p_chunk );

				if ( l_return )
				{
					p_value = Castor::Path{ Castor::string::string_cast< xchar >( l_pChar.data() ) };
				}
			}

			return l_return;
		}
	};
}

#endif
