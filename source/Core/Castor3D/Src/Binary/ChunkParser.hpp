/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		static inline bool Parse( uint8_t * p_values, size_t p_size, BinaryChunk & p_chunk )
		{
			bool l_result = p_chunk.CheckAvailable( uint32_t( p_size ) );

			if ( l_result )
			{
				p_chunk.Get( p_values, uint32_t( p_size ) );
			}

			return l_result;
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
		static inline bool Parse( T * p_values, size_t p_count, BinaryChunk & p_chunk )
		{
			bool l_result{ ChunkParserBase::Parse( reinterpret_cast< uint8_t * >( p_values )
				, p_count * sizeof( T )
				, p_chunk ) };

			for ( uint32_t i = 0; i < p_count; ++i )
			{
				PrepareChunkData( *p_values++ );
			}

			return l_result;
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
			bool l_result{ ChunkParserBase::Parse( GetBuffer( p_value )
				, uint32_t( GetDataSize( p_value ) )
				, p_chunk ) };
			PrepareChunkData( p_value );
			return l_result;
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
			bool l_result = p_chunk.CheckAvailable( 1 );
			uint32_t l_size = p_chunk.GetRemaining();

			if ( l_result )
			{
				std::vector< char > l_buffer( l_size + 1, 0 );
				l_result = ChunkParserBase::Parse( reinterpret_cast< uint8_t * >( l_buffer.data() )
					, l_size
					, p_chunk );

				if ( l_result )
				{
					p_value = Castor::string::string_cast< xchar >( l_buffer.data() );
				}
			}

			return l_result;
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
			bool l_result = p_chunk.CheckAvailable( 1 );
			uint32_t l_size = p_chunk.GetRemaining();

			if ( l_result )
			{
				std::vector< char > l_buffer( l_size + 1, 0 );
				l_result = ChunkParserBase::Parse( reinterpret_cast< uint8_t * >( l_buffer.data() )
					, l_size
					, p_chunk );

				if ( l_result )
				{
					p_value = Castor::Path{ Castor::string::string_cast< xchar >( l_buffer.data() ) };
				}
			}

			return l_result;
		}
	};
}

#endif
