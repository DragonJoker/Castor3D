/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ChunkWriter_H___
#define ___C3D_ChunkWriter_H___

#include "Castor3D/Binary/BinaryChunk.hpp"

namespace castor3d
{
	class ChunkWriterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	begin	The values begin
		 *\param[in]	end		The values end
		 *\param[in]	type	The subchunk type
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	begin	Le début des valeurs
		 *\param[in]	end		La fin des valeurs
		 *\param[in]	type	Le type du subchunk
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( uint8_t const * begin
			, uint8_t const * end
			, ChunkType type
			, BinaryChunk & chunk )
		{
			bool result = true;

			try
			{
				BinaryChunk schunk{ type };
				schunk.setData( begin, end );
				result = chunk.addSubChunk( schunk );
			}
			catch ( ... )
			{
				result = false;
			}

			return result;
		}
	};
	/**
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
		 *\param[in]	begin	The values begin
		 *\param[in]	end		The values end
		 *\param[in]	type	The subchunk type
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	begin	Le début des valeurs
		 *\param[in]	end		La fin de valeurs
		 *\param[in]	type	Le type du subchunk
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( T const * begin
			, T const * end
			, ChunkType type
			, BinaryChunk & chunk )
		{
			std::vector< T > values{ begin, end };

			for ( auto & value : values )
			{
				prepareChunkData( value );
			}

			return ChunkWriterBase::write( reinterpret_cast< uint8_t const * >( values.data() )
				, reinterpret_cast< uint8_t const * >( values.data() + values.size() )
				, type
				, chunk );
		}
		/**
		 *\~english
		 *\brief		Writes a subchunk value into a chunk
		 *\param[in]	value	The value
		 *\param[in]	type	The subchunk type
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	value	La valeur
		 *\param[in]	type	Le type du subchunk
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( T const & value
			, ChunkType type
			, BinaryChunk & chunk )
		{
			auto cvalue = value;
			prepareChunkData( cvalue );
			auto begin = getBuffer( cvalue );
			auto end = begin + getDataSize( cvalue );
			return ChunkWriterBase::write( begin, end, type, chunk );
		}
	};
	/**
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
		 *\param[in]	value	The value
		 *\param[in]	type	The subchunk type
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	value	La valeur
		 *\param[in]	type	Le type du subchunk
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( castor::String const & value
			, ChunkType type
			, BinaryChunk & chunk )
		{
			bool result = true;

			try
			{
				auto svalue = castor::string::stringCast< char >( value );
				auto buffer = reinterpret_cast< uint8_t const * >( svalue.data() );
				ChunkWriterBase::write( buffer, buffer + svalue.size(), type, chunk );
			}
			catch ( ... )
			{
				result = false;
			}

			return result;
		}
	};
	/**
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
		 *\param[in]	value	The value
		 *\param[in]	type	The subchunk type
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	value	La valeur
		 *\param[in]	type	Le type du subchunk
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool write( castor::Path const & value
			, ChunkType type
			, BinaryChunk & chunk )
		{
			bool result = true;

			try
			{
				auto svalue = castor::string::stringCast< char >( value );
				auto buffer = reinterpret_cast< uint8_t const * >( svalue.data() );
				ChunkWriterBase::write( buffer, buffer + svalue.size(), type, chunk );
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
