/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ChunkParser_H___
#define ___C3D_ChunkParser_H___

#include "Castor3D/Binary/BinaryChunk.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	class ChunkParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	values	Receives the parsed values
		 *\param[out]	size	The values size
		 *\param[in]	chunk	The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	values	Reçoit les valeurs
		 *\param[out]	size	La taille des valeurs
		 *\param[in]	chunk	Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool parse( uint8_t * values
			, size_t size
			, BinaryChunk & chunk )
		{
			bool result = chunk.checkAvailable( uint32_t( size ) );

			if ( result )
			{
				chunk.get( values, uint32_t( size ) );
			}

			return result;
		}
	};
	/**
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
		 *\param[out]	values	Receives the parsed values
		 *\param[out]	count	The values count
		 *\param[in]	chunk	The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	values	Reçoit les valeurs
		 *\param[out]	count	Le compte des valeurs
		 *\param[in]	chunk	Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool parse( T * values
			, size_t count
			, BinaryChunk & chunk )
		{
			if ( count == 0u )
			{
				return true;
			}

			bool result{ ChunkParserBase::parse( BytePtr( values )
				, count * sizeof( T )
				, chunk ) };

			for ( uint32_t i = 0; i < count; ++i )
			{
				prepareChunkDataT( &chunk, *values++ );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	value	Receives the parsed value
		 *\param[in]	chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	value	Reçoit la valeur
		 *\param[in]	chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool parse( T & value
			, BinaryChunk & chunk )
		{
			bool result{ ChunkParserBase::parse( getBuffer( value )
				, uint32_t( getDataSize( value ) )
				, chunk ) };
			prepareChunkDataT( &chunk, value );
			return result;
		}
	};
	/**
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
		 *\param[out]	value	Receives the parsed value
		 *\param[in]	chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	value	Reçoit la valeur
		 *\param[in]	chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool parse( castor::String & value
			, BinaryChunk & chunk )
		{
			bool result = chunk.checkAvailable( 1 );
			uint32_t size = chunk.getRemaining();

			if ( result )
			{
				std::vector< char > buffer( size + 1ULL, 0 );
				result = ChunkParserBase::parse( BytePtr( buffer.data() )
					, size
					, chunk );

				if ( result )
				{
					value = castor::string::stringCast< xchar >( buffer.data() );
				}
			}

			return result;
		}
	};
	/**
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
		 *\param[out]	value	Receives the parsed value
		 *\param[in]	chunk	The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	value	Reçoit la valeur
		 *\param[in]	chunk	Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool parse( castor::Path & value
			, BinaryChunk & chunk )
		{
			bool result = chunk.checkAvailable( 1 );
			uint32_t size = chunk.getRemaining();

			if ( result )
			{
				std::vector< char > buffer( size + 1ULL, 0 );
				result = ChunkParserBase::parse( BytePtr( buffer.data() )
					, size
					, chunk );

				if ( result )
				{
					value = castor::Path{ castor::string::stringCast< xchar >( buffer.data() ) };
				}
			}

			return result;
		}
	};
}

#endif
