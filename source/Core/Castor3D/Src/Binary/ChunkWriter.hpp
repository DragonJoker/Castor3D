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
#ifndef ___C3D_CHUNK_WRITER_H___
#define ___C3D_CHUNK_WRITER_H___

#include "BinaryChunk.hpp"
#include "ChunkData.hpp"

namespace Castor3D
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
		static inline bool Write( uint8_t const * p_begin
			, uint8_t const * p_end
			, ChunkType p_type
			, BinaryChunk & p_chunk )
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
		static inline bool Write( T const * p_begin
			, T const * p_end
			, ChunkType p_type
			, BinaryChunk & p_chunk )
		{
			std::vector< T > l_values{ p_begin, p_end };

			for ( auto & l_value : l_values )
			{
				PrepareChunkData( l_value );
			}

			return ChunkWriterBase::Write( reinterpret_cast< uint8_t const * >( l_values.data() )
				, reinterpret_cast< uint8_t const * >( l_values.data() + l_values.size() )
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
		static inline bool Write( T const & p_value
			, ChunkType p_type
			, BinaryChunk & p_chunk )
		{
			auto l_value = p_value;
			PrepareChunkData( l_value );
			auto l_begin = GetBuffer( l_value );
			auto l_end = l_begin + GetDataSize( l_value );
			return ChunkWriterBase::Write( l_begin, l_end, p_type, p_chunk );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		ChunkWriter specialisation for Castor::String.
	\~french
	\brief		Spécialisation de ChunkWriter pour Castor::String.
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
		static inline bool Write( Castor::String const & p_value
			, ChunkType p_type
			, BinaryChunk & p_chunk )
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
	\version	0.9.0
	\date 		30/05/2016
	\~english
	\brief		ChunkWriter specialisation for Castor::Path.
	\~french
	\brief		Spécialisation de ChunkWriter pour Castor::Path.
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
		static inline bool Write( Castor::Path const & p_value
			, ChunkType p_type
			, BinaryChunk & p_chunk )
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
