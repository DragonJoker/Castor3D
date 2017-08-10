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
#ifndef ___C3D_BINARY_PARSER_H___
#define ___C3D_BINARY_PARSER_H___

#include "ChunkParser.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk writer/reader base class
	\~french
	\brief		Classe de base de lecture/écriture d'un chunk de données binaires
	*/
	template< class TParsed >
	class BinaryParserBase
	{
	public:
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un fichier
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse( TParsed & p_obj, castor::BinaryFile & p_file )
		{
			BinaryChunk header;
			bool result = header.read( p_file );

			if ( header.getChunkType() != ChunkType::eCmshFile )
			{
				castor::Logger::logError( cuT( "Not a valid CMSH file." ) );
				result = false;
			}

			if ( result )
			{
				result = doParseHeader( header );
			}

			if ( result )
			{
				result = header.checkAvailable( 1 );
			}

			BinaryChunk chunk;

			if ( result )
			{
				result = header.getSubChunk( chunk );
			}

			if ( result )
			{
				result = parse( p_obj, chunk );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse( TParsed & p_obj, BinaryChunk & p_chunk )
		{
			bool result = true;

			if ( p_chunk.getChunkType() == ChunkTyper< TParsed >::Value )
			{
				m_chunk = &p_chunk;
			}
			else
			{
				castor::Logger::logError( cuT( "Not a valid chunk for parsed type." ) );
				result = false;
			}

			if ( result )
			{
				result = doParse( p_obj );

				if ( !result )
				{
					m_chunk->endParse();
				}
			}

			return result;
		}

	protected:
		/**
		 *\~english
		 *\brief			Parses the header chunk.
		 *\param[in,out]	p_chunk	The parent chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Lit le chunk d'en-tête.
		 *\param[in,out]	p_chunk	Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool doParseHeader( BinaryChunk & p_chunk )const
		{
			BinaryChunk chunk;
			bool result = p_chunk.getSubChunk( chunk );

			if ( chunk.getChunkType() != ChunkType::eCmshHeader )
			{
				castor::Logger::logError( cuT( "Missing header chunk." ) );
				result = false;
			}

			castor::String name;
			uint32_t version{ 0 };

			while ( result && chunk.checkAvailable( 1 ) )
			{
				BinaryChunk subchunk;
				result = chunk.getSubChunk( subchunk );

				switch ( subchunk.getChunkType() )
				{
				case ChunkType::eName:
					result = doParseChunk( name, subchunk );
					break;

				case ChunkType::eCmshVersion:
					result = doParseChunk( version, subchunk );
					break;
				}
			}

			if ( !result )
			{
				p_chunk.endParse();
			}

			return result;
		}
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
		template< typename T >
		inline bool doParseChunk( T * p_values, size_t p_count, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >::parse( p_values, p_count, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, size_t Count >
		inline bool doParseChunk( T( &p_values )[Count], BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >::parse( p_values, Count, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, size_t Count >
		inline bool doParseChunk( std::array< T, Count > & p_values, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >::parse( p_values.data(), Count, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	p_values	Receives the parsed values
		 *\param[in]	p_chunk		The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	p_values	Reçoit les valeurs
		 *\param[in]	p_chunk		Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		inline bool doParseChunk( std::vector< T > & p_values, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >::parse( p_values.data(), p_values.size(), p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value from a chunk
		 *\param[out]	p_value	Receives the parsed value
		 *\param[in]	p_chunk		The chunk containing the value
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère une valeur à partir d'un chunk
		 *\param[out]	p_value	Reçoit la valeur
		 *\param[in]	p_chunk		Le chunk contenant la valeur
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		inline bool doParseChunk( T & p_value, BinaryChunk & p_chunk )const
		{
			return ChunkParser< T >::parse( p_value, p_chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a subchunk.
		 *\param[out]	p_chunk	Receives the subchunk.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Récupère un sous-chunk.
		 *\param[out]	p_chunk	Reçoit le sous-chunk.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		inline bool doGetSubChunk( BinaryChunk & p_chunk )
		{
			REQUIRE( m_chunk );
			bool result = m_chunk->checkAvailable( 1 );

			if ( result )
			{
				result = m_chunk->getSubChunk( p_chunk );
			}

			return result;
		}

	private:
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	p_obj	The object to read
		 *\param[in]	p_chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	p_obj	L'objet à lire
		 *\param[in]	p_chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API virtual bool doParse( TParsed & p_obj ) = 0;

	protected:
		//!\~english	The writer's chunk.
		//!\~french		Le chunk du writer.
		BinaryChunk * m_chunk{ nullptr };
	};
	template< class TParsed >
	class BinaryParser;
}

#endif
