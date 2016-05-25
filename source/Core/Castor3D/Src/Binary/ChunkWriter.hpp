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
#ifndef ___C3D_CHUNK_WRITER_H___
#define ___C3D_CHUNK_WRITER_H___

#include "BinaryChunk.hpp"
#include "ChunkData.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk filler
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
		static inline bool Write( uint8_t const * p_begin, uint8_t const * p_end, eCHUNK_TYPE p_type, BinaryChunk & p_chunk )
		{
			bool l_return = true;

			try
			{
				BinaryChunk l_chunk{ p_type };
				l_chunk.SetData( p_begin, p_end );
				l_return = p_chunk.AddSubChunk( l_chunk );
			}
			catch ( ... )
			{
				l_return = false;
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		Chunk filler
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
		 *\param[in]	p_values	The values
		 *\param[in]	p_count		The values count
		 *\param[in]	p_type		The subchunk type
		 *\param[in]	p_chunk		The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit une valeur d'un subchunk dans un chunk
		 *\param[in]	p_values	Les valeurs
		 *\param[in]	p_count		Le nombre de valeurs
		 *\param[in]	p_type		Le type du subchunk
		 *\param[in]	p_chunk		Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		static inline bool Write( T const * p_begin, T const * p_end, eCHUNK_TYPE p_type, BinaryChunk & p_chunk )
		{
			return ChunkWriterBase::Write( reinterpret_cast< uint8_t const * >( p_begin ), reinterpret_cast< uint8_t const * >( p_end ), p_type, p_chunk );
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
		static inline bool Write( T const & p_value, eCHUNK_TYPE p_type, BinaryChunk & p_chunk )
		{
			auto l_begin = ChunkData< T >::GetBuffer( p_value );
			auto l_end = l_begin + ChunkData< T >::GetDataSize( p_value );
			return ChunkWriterBase::Write( l_begin, l_end, p_type, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::String.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::String.
	*/
	template<>
	class ChunkWriter< Castor::String >
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
		static inline bool Write( Castor::String const & p_value, eCHUNK_TYPE p_type, BinaryChunk & p_chunk )
		{
			bool l_return = true;

			try
			{
				auto l_value = Castor::string::string_cast< char >( p_value );
				auto l_buffer = reinterpret_cast< uint8_t const * >( l_value.data() );
				ChunkWriterBase::Write( l_buffer, l_buffer + l_value.size(), p_type, p_chunk );
			}
			catch ( ... )
			{
				l_return = false;
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/04/2014
	\~english
	\brief		ChunkFiller specialisation for Castor::Path.
	\~french
	\brief		Spécialisation de ChunkFiller pour Castor::Path.
	*/
	template<>
	class ChunkWriter< Castor::Path >
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
		static inline bool Write( Castor::Path const & p_value, eCHUNK_TYPE p_type, BinaryChunk & p_chunk )
		{
			bool l_return = true;

			try
			{
				auto l_value = Castor::string::string_cast< char >( p_value );
				auto l_buffer = reinterpret_cast< uint8_t const * >( l_value.data() );
				ChunkWriterBase::Write( l_buffer, l_buffer + l_value.size(), p_type, p_chunk );
			}
			catch ( ... )
			{
				l_return = false;
			}

			return l_return;
		}
	};
}

#endif
