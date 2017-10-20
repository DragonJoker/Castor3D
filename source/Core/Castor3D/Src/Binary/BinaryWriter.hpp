/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BINARY_WRITER_H___
#define ___C3D_BINARY_WRITER_H___

#include "ChunkWriter.hpp"
#include "Miscellaneous/Version.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk writer/reader base class.
	\~french
	\brief		Classe de base de lecture/écriture d'un chunk de données binaires.
	*/
	template< class TWritten >
	class BinaryWriterBase
	{
	public:
		/**
		 *\~english
		 *\brief			Writes an object to a file.
		 *\param[in]		p_obj	The object to write.
		 *\param[in,out]	p_file	The file.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Fonction d'écriture dans un fichier.
		 *\param[in]		p_obj	L'objet à écrire.
		 *\param[in,out]	p_file	Le fichier.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool write( TWritten const & p_obj, castor::BinaryFile & p_file )
		{
			BinaryChunk chunk{ ChunkType::eCmshFile };
			bool result = doWriteHeader( chunk );

			if ( result )
			{
				result = write( p_obj, chunk );
			}

			if ( result )
			{
				result = chunk.write( p_file );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief			To chunk writer function.
		 *\param[in]		p_obj	The object to write.
		 *\param[in,out]	p_chunk	The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Fonction d'écriture dans un chunk.
		 *\param[in]		p_obj	L'objet à écrire.
		 *\param[in,out]	p_chunk	Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool write( TWritten const & p_obj, BinaryChunk & p_chunk )
		{
			bool result{ doWrite( p_obj ) };

			if ( result )
			{
				m_chunk.finalise();
				p_chunk.addSubChunk( m_chunk );
			}

			return result;
		}

	protected:
		/**
		 *\~english
		 *\brief			Writes the header chunk.
		 *\param[in,out]	p_chunk	The parent chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une le chunk d'en-tête.
		 *\param[in,out]	p_chunk	Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool doWriteHeader( BinaryChunk & p_chunk )const
		{
			BinaryChunk chunk{ ChunkType::eCmshHeader };
			bool result = doWriteChunk( CMSH_VERSION, ChunkType::eCmshVersion, chunk );

			if ( result )
			{
				castor::StringStream stream;
				stream << cuT( "Castor 3D - Version " ) << castor3d::Version{};
				result = doWriteChunk( stream.str(), ChunkType::eName, chunk );
			}

			if ( result )
			{
				chunk.finalise();
				result = p_chunk.addSubChunk( chunk );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_values	The values.
		 *\param[in]		p_count		The values count.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_values	Les valeurs.
		 *\param[in]		p_count		Le nombre de valeurs.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( T const * p_values, size_t p_count, ChunkType p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::write( p_values, p_values + p_count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_values	The values.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_values	Les valeurs.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T, size_t Count >
		inline bool doWriteChunk( T const( &p_values )[Count], ChunkType p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::write( p_values, p_values + Count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_values	The values.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_values	Les valeurs.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T, size_t Count >
		inline bool doWriteChunk( std::array< T, Count > const & p_values, ChunkType p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::write( p_values.data(), p_values.data() + Count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_values	The values.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_values	Les valeurs.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( std::vector< T > const & p_values, ChunkType p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::write( p_values.data(), p_values.data() + p_values.size(), p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_begin		The values begin.
		 *\param[in]		p_end		The values end.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_begin		Le début des valeurs.
		 *\param[in]		p_end		La fin des valeurs.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( T const * p_begin, T const * p_end, ChunkType p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::write( p_begin, p_end, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_value		The value.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_value		La valeur.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool doWriteChunk( T const & p_value, ChunkType p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::write( p_value, p_chunkType, p_chunk );
		}

	private:
		/**
		 *\~english
		 *\brief			Writes the object to the writer's chunk.
		 *\param[in]		p_obj	The object to write.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit l'objet dans le chunk du writer.
		 *\param[in]		p_obj	L'objet à écrire.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		C3D_API virtual bool doWrite( TWritten const & p_obj ) = 0;

	protected:
		//!\~english	The writer's chunk.
		//!\~french		Le chunk du writer.
		BinaryChunk m_chunk{ ChunkTyper< TWritten >::Value };
	};
	template< class TWritten >
	class BinaryWriter;
}

#endif
