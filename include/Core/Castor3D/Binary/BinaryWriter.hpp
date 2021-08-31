/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryWriter_H___
#define ___C3D_BinaryWriter_H___

#include "Castor3D/Binary/ChunkWriter.hpp"
#include "Castor3D/Miscellaneous/Version.hpp"

namespace castor3d
{
	template< class TWritten >
	class BinaryWriterBase
	{
	public:
		virtual ~BinaryWriterBase() = default;
		/**
		 *\~english
		 *\brief			Writes an object to a file.
		 *\param[in]		obj		The object to write.
		 *\param[in,out]	file	The file.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Fonction d'écriture dans un fichier.
		 *\param[in]		obj		L'objet à écrire.
		 *\param[in,out]	file	Le fichier.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool write( TWritten const & obj
			, castor::BinaryFile & file )
		{
			BinaryChunk chunk{ ChunkType::eCmshFile };
			bool result = doWriteHeader( chunk );

			if ( result )
			{
				result = write( obj, chunk );
			}

			if ( result )
			{
				result = chunk.write( file );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief			To chunk writer function.
		 *\param[in]		obj		The object to write.
		 *\param[in,out]	chunk	The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Fonction d'écriture dans un chunk.
		 *\param[in]		obj		L'objet à écrire.
		 *\param[in,out]	chunk	Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool write( TWritten const & obj
			, BinaryChunk & chunk )
		{
			bool result{ doWrite( obj ) };

			if ( result )
			{
				m_chunk.finalise();
				chunk.addSubChunk( m_chunk );
			}

			return result;
		}

	protected:
		/**
		 *\~english
		 *\brief			Writes the header chunk.
		 *\param[in,out]	chunk	The parent chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une le chunk d'en-tête.
		 *\param[in,out]	chunk	Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool doWriteHeader( BinaryChunk & chunk )const
		{
			BinaryChunk schunk{ ChunkType::eCmshHeader };
			bool result = doWriteChunk( CurrentCmshVersion, ChunkType::eCmshVersion, schunk );

			if ( result )
			{
				castor::StringStream stream{ castor::makeStringStream() };
				stream << cuT( "Castor 3D - Version " ) << castor3d::Version{};
				result = doWriteChunk( stream.str(), ChunkType::eName, schunk );
			}

			if ( result )
			{
				schunk.finalise();
				result = chunk.addSubChunk( schunk );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		values		The values.
		 *\param[in]		count		The values count.
		 *\param[in]		chunkType	The subchunk type.
		 *\param[in,out]	chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		values		Les valeurs.
		 *\param[in]		count		Le nombre de valeurs.
		 *\param[in]		chunkType	Le type du subchunk.
		 *\param[in,out]	chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( T const * values
			, size_t count
			, ChunkType chunkType
			, BinaryChunk & chunk )const
		{
			return ChunkWriter< T >::write( values, values + count, chunkType, chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		values		The values.
		 *\param[in]		chunkType	The subchunk type.
		 *\param[in,out]	chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		values		Les valeurs.
		 *\param[in]		chunkType	Le type du subchunk.
		 *\param[in,out]	chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T, size_t Count >
		inline bool doWriteChunk( T const( &values )[Count]
			, ChunkType chunkType
			, BinaryChunk & chunk )const
		{
			return ChunkWriter< T >::write( values, values + Count, chunkType, chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		values		The values.
		 *\param[in]		chunkType	The subchunk type.
		 *\param[in,out]	chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		values		Les valeurs.
		 *\param[in]		chunkType	Le type du subchunk.
		 *\param[in,out]	chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T, size_t Count >
		inline bool doWriteChunk( std::array< T, Count > const & values
			, ChunkType chunkType
			, BinaryChunk & chunk )const
		{
			return ChunkWriter< T >::write( values.data(), values.data() + Count, chunkType, chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		values		The values.
		 *\param[in]		chunkType	The subchunk type.
		 *\param[in,out]	chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		values		Les valeurs.
		 *\param[in]		chunkType	Le type du subchunk.
		 *\param[in,out]	chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( std::vector< T > const & values
			, ChunkType chunkType
			, BinaryChunk & chunk )const
		{
			return ChunkWriter< T >::write( values.data(), values.data() + values.size(), chunkType, chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		begin		The values begin.
		 *\param[in]		end			The values end.
		 *\param[in]		chunkType	The subchunk type.
		 *\param[in,out]	chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		begin		Le début des valeurs.
		 *\param[in]		end			La fin des valeurs.
		 *\param[in]		chunkType	Le type du subchunk.
		 *\param[in,out]	chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( T const * begin
			, T const * end
			, ChunkType chunkType
			, BinaryChunk & chunk )const
		{
			return ChunkWriter< T >::write( begin, end, chunkType, chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		value		The value.
		 *\param[in]		chunkType	The subchunk type.
		 *\param[in,out]	chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		value		La valeur.
		 *\param[in]		chunkType	Le type du subchunk.
		 *\param[in,out]	chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( T const & value
			, ChunkType chunkType
			, BinaryChunk & chunk )const
		{
			return ChunkWriter< T >::write( value, chunkType, chunk );
		}

	private:
		/**
		 *\~english
		 *\brief			Writes the object to the writer's chunk.
		 *\param[in]		obj	The object to write.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit l'objet dans le chunk du writer.
		 *\param[in]		obj	L'objet à écrire.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		C3D_API virtual bool doWrite( TWritten const & obj ) = 0;

	protected:
		//!\~english	The writer's chunk.
		//!\~french		Le chunk du writer.
		BinaryChunk m_chunk{ ChunkTyper< TWritten >::Value };
	};
}

#endif
