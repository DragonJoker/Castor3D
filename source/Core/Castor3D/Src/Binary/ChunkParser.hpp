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

namespace castor3d
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
		static inline bool parse( uint8_t * p_values, size_t p_size, BinaryChunk & p_chunk )
		{
			bool result = p_chunk.checkAvailable( uint32_t( p_size ) );

			if ( result )
			{
				p_chunk.get( p_values, uint32_t( p_size ) );
			}

			return result;
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
		static inline bool parse( T * p_values, size_t p_count, BinaryChunk & p_chunk )
		{
			bool result{ ChunkParserBase::parse( reinterpret_cast< uint8_t * >( p_values )
				, p_count * sizeof( T )
				, p_chunk ) };

			for ( uint32_t i = 0; i < p_count; ++i )
			{
				prepareChunkData( *p_values++ );
			}

			return result;
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
		static inline bool parse( T & p_value, BinaryChunk & p_chunk )
		{
			bool result{ ChunkParserBase::parse( getBuffer( p_value )
				, uint32_t( getDataSize( p_value ) )
				, p_chunk ) };
			prepareChunkData( p_value );
			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for castor::String
	\~french
	\brief		Spécialisation de ChunkParser pour castor::String
	*/
	template<>
	class ChunkParser< castor::String >
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
		static inline bool parse( castor::String & p_value, BinaryChunk & p_chunk )
		{
			bool result = p_chunk.checkAvailable( 1 );
			uint32_t size = p_chunk.getRemaining();

			if ( result )
			{
				std::vector< char > buffer( size + 1, 0 );
				result = ChunkParserBase::parse( reinterpret_cast< uint8_t * >( buffer.data() )
					, size
					, p_chunk );

				if ( result )
				{
					p_value = castor::string::stringCast< xchar >( buffer.data() );
				}
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkParser specialisation for castor::Path
	\~french
	\brief		Spécialisation de ChunkParser pour castor::Path
	*/
	template<>
	class ChunkParser< castor::Path >
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
		static inline bool parse( castor::Path & p_value, BinaryChunk & p_chunk )
		{
			bool result = p_chunk.checkAvailable( 1 );
			uint32_t size = p_chunk.getRemaining();

			if ( result )
			{
				std::vector< char > buffer( size + 1, 0 );
				result = ChunkParserBase::parse( reinterpret_cast< uint8_t * >( buffer.data() )
					, size
					, p_chunk );

				if ( result )
				{
					p_value = castor::Path{ castor::string::stringCast< xchar >( buffer.data() ) };
				}
			}

			return result;
		}
	};
}

#endif
