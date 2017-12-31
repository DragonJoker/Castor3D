/*
See LICENSE file in root folder
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
