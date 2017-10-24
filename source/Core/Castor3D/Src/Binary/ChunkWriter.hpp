/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CHUNK_WRITER_H___
#define ___C3D_CHUNK_WRITER_H___

#include "BinaryChunk.hpp"
#include "ChunkData.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk writer
	\~french
	\brief		Remplisseur de chunk
	*/
	class ChunkWriterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_begin	The values begin
		 *\param[in]	p_end	The values end
		 *\param[in]	p_type	The subchunk type
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_begin	Le début des valeurs
		 *\param[in]	p_end	La fin des valeurs
		 *\param[in]	p_type	Le type du subchunk
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( uint8_t const * p_begin
			, uint8_t const * p_end
			, ChunkType p_type
			, BinaryChunk & p_chunk )
		{
			bool result = true;

			try
			{
				BinaryChunk chunk{ p_type };
				chunk.setData( p_begin, p_end );
				result = p_chunk.addSubChunk( chunk );
			}
			catch ( ... )
			{
				result = false;
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		Chunk data writer
	\~french
	\brief		Remplisseur de chunk
	*/
	template< typename T >
	class ChunkWriter
		: public ChunkWriterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_begin	The values begin
		 *\param[in]	p_end	The values end
		 *\param[in]	p_type	The subchunk type
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_begin	Le début des valeurs
		 *\param[in]	p_end	La fin de valeurs
		 *\param[in]	p_type	Le type du subchunk
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( T const * p_begin
			, T const * p_end
			, ChunkType p_type
			, BinaryChunk & p_chunk )
		{
			std::vector< T > values{ p_begin, p_end };

			for ( auto & value : values )
			{
				prepareChunkData( value );
			}

			return ChunkWriterBase::write( reinterpret_cast< uint8_t const * >( values.data() )
				, reinterpret_cast< uint8_t const * >( values.data() + values.size() )
				, p_type
				, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value	The value
		 *\param[in]	p_type	The subchunk type
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value	La valeur
		 *\param[in]	p_type	Le type du subchunk
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( T const & p_value
			, ChunkType p_type
			, BinaryChunk & p_chunk )
		{
			auto value = p_value;
			prepareChunkData( value );
			auto begin = getBuffer( value );
			auto end = begin + getDataSize( value );
			return ChunkWriterBase::write( begin, end, p_type, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		ChunkWriter specialisation for castor::String.
	\~french
	\brief		Spécialisation de ChunkWriter pour castor::String.
	*/
	template<>
	class ChunkWriter< castor::String >
		: public ChunkWriterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value	The value
		 *\param[in]	p_type	The subchunk type
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value	La valeur
		 *\param[in]	p_type	Le type du subchunk
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( castor::String const & p_value
			, ChunkType p_type
			, BinaryChunk & p_chunk )
		{
			bool result = true;

			try
			{
				auto value = castor::string::stringCast< char >( p_value );
				auto buffer = reinterpret_cast< uint8_t const * >( value.data() );
				ChunkWriterBase::write( buffer, buffer + value.size(), p_type, p_chunk );
			}
			catch ( ... )
			{
				result = false;
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		ChunkWriter specialisation for castor::Path.
	\~french
	\brief		Spécialisation de ChunkWriter pour castor::Path.
	*/
	template<>
	class ChunkWriter< castor::Path >
		: public ChunkWriterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	p_value	The value
		 *\param[in]	p_type	The subchunk type
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_value	La valeur
		 *\param[in]	p_type	Le type du subchunk
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( castor::Path const & p_value
			, ChunkType p_type
			, BinaryChunk & p_chunk )
		{
			bool result = true;

			try
			{
				auto value = castor::string::stringCast< char >( p_value );
				auto buffer = reinterpret_cast< uint8_t const * >( value.data() );
				ChunkWriterBase::write( buffer, buffer + value.size(), p_type, p_chunk );
			}
			catch ( ... )
			{
				result = false;
			}

			return result;
		}
	};
}

#endif
