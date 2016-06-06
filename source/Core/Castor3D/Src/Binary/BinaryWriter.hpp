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
#ifndef ___C3D_BINARY_WRITER_H___
#define ___C3D_BINARY_WRITER_H___

#include "ChunkWriter.hpp"
#include "Miscellaneous/Version.hpp"

namespace Castor3D
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
		inline bool Write( TWritten const & p_obj, Castor::BinaryFile & p_file )
		{
			BinaryChunk l_chunk{ eCHUNK_TYPE_CMSH_FILE };
			bool l_return = DoWriteHeader( l_chunk );

			if ( l_return )
			{
				l_return = Write( p_obj, l_chunk );
			}

			if ( l_return )
			{
				l_return = l_chunk.Write( p_file );
			}

			return l_return;
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
		inline bool Write( TWritten const & p_obj, BinaryChunk & p_chunk )
		{
			bool l_return{ DoWrite( p_obj ) };
			
			if ( l_return )
			{
				m_chunk.Finalise();
				p_chunk.AddSubChunk( m_chunk );
			}

			return l_return;
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
		inline bool DoWriteHeader( BinaryChunk & p_chunk )const
		{
			BinaryChunk l_chunk{ eCHUNK_TYPE_CMSH_HEADER };
			bool l_return = DoWriteChunk( CMSH_VERSION, eCHUNK_TYPE_CMSH_VERSION, l_chunk );

			if ( l_return )
			{
				Castor::StringStream l_stream;
				l_stream << cuT( "Castor 3D - Version " ) << Castor3D::Version{};
				l_return = DoWriteChunk( l_stream.str(), eCHUNK_TYPE_NAME, l_chunk );
			}

			if ( l_return )
			{
				l_chunk.Finalise();
				l_return = p_chunk.AddSubChunk( l_chunk );
			}

			return l_return;
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
		inline bool DoWriteChunk( T const * p_values, size_t p_count, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_values, p_values + p_count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_value		The values.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_value		Les valeurs.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T, size_t Count >
		inline bool DoWriteChunk( T const( &p_values )[Count], eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_values, p_values + Count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_value		The values.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]		p_value		Les valeurs.
		 *\param[in]		p_chunkType	Le type du subchunk.
		 *\param[in,out]	p_chunk		Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		template< typename T, size_t Count >
		inline bool DoWriteChunk( std::array< T, Count > const & p_values, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_values.data(), p_values.data() + Count, p_chunkType, p_chunk );
		}
		/**
		 *\~english
		 *\brief			Writes a subchunk value into a chunk.
		 *\param[in]		p_value		The values.
		 *\param[in]		p_chunkType	The subchunk type.
		 *\param[in,out]	p_chunk		The chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk.
		 *\param[in]	p_value		Les valeurs.
		 *\param[in]	p_chunkType	Le type du subchunk.
		 *\param[in]	p_chunk		Le chunk.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		template< typename T >
		inline bool DoWriteChunk( std::vector< T > const & p_values, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_values.data(), p_values.data() + p_values.size(), p_chunkType, p_chunk );
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
		inline bool DoWriteChunk( T const * p_begin, T const * p_end, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_begin, p_end, p_chunkType, p_chunk );
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
		inline bool DoWriteChunk( T const & p_value, eCHUNK_TYPE p_chunkType, BinaryChunk & p_chunk )const
		{
			return ChunkWriter< T >::Write( p_value, p_chunkType, p_chunk );
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
		C3D_API virtual bool DoWrite( TWritten const & p_obj ) = 0;

	protected:
		//!\~english	The writer's chunk.
		//!\~french		Le chunk du writer.
		BinaryChunk m_chunk{ ChunkTyper< TWritten >::Value };
	};
	template< class TWritten >
	class BinaryWriter;
}

#endif
