/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryParser_H___
#define ___C3D_BinaryParser_H___

#pragma GCC diagnostic ignored "-Wundefined-var-template"

#include "Castor3D/Binary/ChunkParser.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/Version.hpp"

namespace castor3d
{
	template< class TParsed >
	class BinaryParserBase
	{
		template< typename T >
		friend class BinaryParserBase;

	public:
		virtual ~BinaryParserBase() = default;
		/**
		 *\~english
		 *\brief		Creates a binary parser.
		 *\~french
		 *\brief		Crée un parser binaire.
		 */
		template< typename T >
		inline BinaryParser< T > createBinaryParser()
		{
			BinaryParser< T > parser;
			parser.m_fileVersion = m_fileVersion;
			return parser;
		}
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[out]	obj		The object to read
		 *\param[in]	file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un fichier
		 *\param[out]	obj		L'objet à lire
		 *\param[in]	file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse( TParsed & obj
			, castor::BinaryFile & file )
		{
			BinaryChunk header;
			bool result = header.read( file );

			if ( header.getChunkType() != ChunkType::eCmshFile )
			{
				result = false;
				checkError( result, "Not a valid CMSH file." );
			}

			if ( result )
			{
				result = doParseHeader( header );
			}

			if ( result )
			{
				result = header.checkAvailable( 1 );
				checkError( result, "No more data in chunk." );
			}

			BinaryChunk chunk;

			if ( result )
			{
				result = header.getSubChunk( chunk );
				checkError( result, "Couldn't retrieve subchunk." );
			}

			if ( result )
			{
				result = parse( obj, chunk );
				checkError( result, "Couldn't parse chunk." );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	obj		The object to read
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	obj		L'objet à lire
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse( TParsed & obj
			, BinaryChunk & chunk )
		{
			bool result = true;

			if ( chunk.getChunkType() == ChunkTyper< TParsed >::Value )
			{
				m_chunk = &chunk;
			}
			else
			{
				result = false;
				checkError( result, "Not a valid chunk for parsed type." );
			}

			if ( result )
			{
				result = doParse_v1_1( obj );

				if ( result )
				{
					m_chunk->resetParse();
					result = doParse_v1_2( obj );
				}

				if ( result )
				{
					m_chunk->resetParse();
					result = doParse_v1_3( obj );
				}

				if ( result )
				{
					m_chunk->resetParse();
					result = doParse_v1_4( obj );
				}

				if ( result )
				{
					m_chunk->resetParse();
					result = doParse( obj );
				}

				if ( !result )
				{
					m_chunk->endParse();
				}
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	obj		The object to read
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	obj		L'objet à lire
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse_v1_1( TParsed & obj
			, BinaryChunk & chunk )
		{
			bool result = true;

			if ( chunk.getChunkType() == ChunkTyper< TParsed >::Value )
			{
				m_chunk = &chunk;
			}
			else
			{
				result = false;
				checkError( result, "Not a valid chunk for parsed type." );
			}

			if ( result )
			{
				result = doParse_v1_1( obj );
				checkError( result, "Couldn't parse chunk." );

				if ( !result )
				{
					m_chunk->endParse();
				}
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	obj		The object to read
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	obj		L'objet à lire
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse_v1_2( TParsed & obj
			, BinaryChunk & chunk )
		{
			bool result = true;

			if ( chunk.getChunkType() == ChunkTyper< TParsed >::Value )
			{
				m_chunk = &chunk;
			}
			else
			{
				result = false;
				checkError( result, "Not a valid chunk for parsed type." );
			}

			if ( result )
			{
				result = doParse_v1_2( obj );
				checkError( result, "Couldn't parse chunk." );

				if ( !result )
				{
					m_chunk->endParse();
				}
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	obj		The object to read
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	obj		L'objet à lire
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse_v1_3( TParsed & obj
			, BinaryChunk & chunk )
		{
			bool result = true;

			if ( chunk.getChunkType() == ChunkTyper< TParsed >::Value )
			{
				m_chunk = &chunk;
			}
			else
			{
				result = false;
				checkError( result, "Not a valid chunk for parsed type." );
			}

			if ( result )
			{
				result = doParse_v1_3( obj );
				checkError( result, "Couldn't parse chunk." );

				if ( !result )
				{
					m_chunk->endParse();
				}
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	obj		The object to read
		 *\param[in]	chunk	The chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	obj		L'objet à lire
		 *\param[in]	chunk	Le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		inline bool parse_v1_4( TParsed & obj
			, BinaryChunk & chunk )
		{
			bool result = true;

			if ( chunk.getChunkType() == ChunkTyper< TParsed >::Value )
			{
				m_chunk = &chunk;
			}
			else
			{
				result = false;
				checkError( result, "Not a valid chunk for parsed type." );
			}

			if ( result )
			{
				result = doParse_v1_4( obj );
				checkError( result, "Couldn't parse chunk." );

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
		 *\param[in,out]	chunk	The parent chunk.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Lit le chunk d'en-tête.
		 *\param[in,out]	chunk	Le chunk.
		 *\return			\p false si une erreur quelconque est arrivée.
		 */
		inline bool doParseHeader( BinaryChunk & chunk )const
		{
			BinaryChunk schunk;
			bool result = chunk.getSubChunk( schunk );

			if ( schunk.getChunkType() != ChunkType::eCmshHeader )
			{
				result = false;
				checkError( result, "Missing header chunk." );
			}

			castor::String name;
			uint32_t version{ 0 };

			while ( result && schunk.checkAvailable( 1 ) )
			{
				BinaryChunk subchunk;
				result = schunk.getSubChunk( subchunk );
				checkError( result, "Couldn't retrieve subchunk." );

				switch ( subchunk.getChunkType() )
				{
				case ChunkType::eName:
					result = doParseChunk( name, subchunk );
					break;

				case ChunkType::eCmshVersion:
					result = doParseChunk( version, subchunk );
					checkError( result, "Couldn't parse chunk." );

					if ( result )
					{
						m_fileVersion = Version{ int( getCmshMajor( version ) )
							, int( getCmshMinor( version ) )
							, int( getCmshRevision( version ) ) };
						Version latestVersion{ int( getCmshMajor( CurrentCmshVersion ) )
							, int( getCmshMinor( CurrentCmshVersion ) )
							, int( getCmshRevision( CurrentCmshVersion ) ) };

						if ( m_fileVersion < latestVersion )
						{
							log::warn << cuT( "This file is using version " )
								<< m_fileVersion
								<< cuT( ", consider upgrading it to version " )
								<< latestVersion
								<< cuT( "." ) << std::endl;
						}
					}

					break;

				default:
					break;
				}
			}

			if ( !result )
			{
				checkError( result, "Couldn't parse chunk." );
				chunk.endParse();
			}

			return result;
		}
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
		template< typename T >
		inline bool doParseChunk( T * values
			, size_t count
			, BinaryChunk & chunk )const
		{
			return ChunkParser< T >::parse( values, count, chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	values	Receives the parsed values
		 *\param[in]	chunk	The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	values	Reçoit les valeurs
		 *\param[in]	chunk	Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, size_t Count >
		inline bool doParseChunk( T( &values )[Count]
			, BinaryChunk & chunk )const
		{
			return ChunkParser< T >::parse( values, Count, chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	values	Receives the parsed values
		 *\param[in]	chunk	The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	values	Reçoit les valeurs
		 *\param[in]	chunk	Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T, size_t Count >
		inline bool doParseChunk( std::array< T, Count > & values
			, BinaryChunk & chunk )const
		{
			return ChunkParser< T >::parse( values.data(), Count, chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a value array from a chunk
		 *\param[out]	values	Receives the parsed values
		 *\param[in]	chunk	The chunk containing the values
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un tableau de valeurs à partir d'un chunk
		 *\param[out]	values	Reçoit les valeurs
		 *\param[in]	chunk	Le chunk contenant les valeurs
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		template< typename T >
		inline bool doParseChunk( std::vector< T > & values
			, BinaryChunk & chunk )const
		{
			return ChunkParser< T >::parse( values.data(), values.size(), chunk );
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
		template< typename T >
		inline bool doParseChunk( T & value
			, BinaryChunk & chunk )const
		{
			return ChunkParser< T >::parse( value, chunk );
		}
		/**
		 *\~english
		 *\brief		Retrieves a subchunk.
		 *\param[out]	chunk	Receives the subchunk.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Récupère un sous-chunk.
		 *\param[out]	chunk	Reçoit le sous-chunk.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		inline bool doGetSubChunk( BinaryChunk & chunk )
		{
			CU_Require( m_chunk );
			bool result = m_chunk->checkAvailable( 1 );

			if ( result )
			{
				result = m_chunk->getSubChunk( chunk );
			}

			return result;
		}

	private:
		/**
		 *\~english
		 *\brief		From chunk reader function
		 *\param[out]	obj	The object to read
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk
		 *\param[out]	obj	L'objet à lire
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API virtual bool doParse( TParsed & obj ) = 0;
		/**
		 *\~english
		 *\brief		Chunk reader function from a chunk of version 1.1.
		 *\param[out]	obj	The object to read
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk en version 1.1.
		 *\param[out]	obj	L'objet à lire
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API virtual bool doParse_v1_1( TParsed & obj )
		{
			return true;
		}
		/**
		 *\~english
		 *\brief		Chunk reader function from a chunk of version 1.2.
		 *\param[out]	obj	The object to read
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk en version 1.2.
		 *\param[out]	obj	L'objet à lire
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API virtual bool doParse_v1_2( TParsed & obj )
		{
			return true;
		}
		/**
		 *\~english
		 *\brief		Chunk reader function from a chunk of version 1.3.
		 *\param[out]	obj	The object to read
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk en version 1.3.
		 *\param[out]	obj	L'objet à lire
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API virtual bool doParse_v1_3( TParsed & obj )
		{
			return true;
		}
		/**
		 *\~english
		 *\brief		Chunk reader function from a chunk of version 1.4.
		 *\param[out]	obj	The object to read
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un chunk en version 1.4.
		 *\param[out]	obj	L'objet à lire
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API virtual bool doParse_v1_4( TParsed & obj )
		{
			return true;
		}

	protected:
		C3D_API static castor::String Name;

		inline void checkError( bool result
			, castor::String const & text )const
		{
			if ( !result )
			{
				log::error << "BinaryParser< " << Name << " >: " << text << std::endl;
			}
		}

	protected:
		//!\~english	The writer's chunk.
		//!\~french		Le chunk du writer.
		BinaryChunk * m_chunk{ nullptr };
		//!\~english	The chunk version from the file.
		//!\~french		La version du chunk dans le fichier.
		mutable Version m_fileVersion;
	};
}

#endif
