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
#ifndef ___C3D_BINARY_CHUNK_H___
#define ___C3D_BINARY_CHUNK_H___

#include "Castor3DPrerequisites.hpp"
#include "ChunkData.hpp"

#include <cstring>

namespace castor3d
{
	//!\~english	Macro to make a version number.
	//!\~french		Macro pour créer un numéro de version.
#	define	MAKE_CMSH_VERSION( major, minor, revision )\
	((uint32_t( major ) << 24)\
	| (uint32_t( minor ) << 16)\
	| (uint32_t( revision ) << 0))
	//!\~english	The current format version number.
	//!\~french		La version actuelle du format.
	uint32_t const CMSH_VERSION = MAKE_CMSH_VERSION( 0x01, 0x01, 0x0000 );
	//!\~english	A define to ease the declaration of a chunk id.
	//!\~french		Un define pour faciliter la déclaration d'un id de chunk.
#	define MAKE_CHUNK_ID( a, b, c, d, e, f, g, h )\
	((uint64_t( a ) << 56)\
	| (uint64_t( b ) << 48)\
	| (uint64_t( c ) << 40)\
	| (uint64_t( d ) << 32)\
	| (uint64_t( e ) << 24)\
	| (uint64_t( f ) << 16)\
	| (uint64_t( g ) << 8)\
	| (uint64_t( h ) << 0))
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk types enumeration
	\~french
	\brief		Enumération des types de chunks de données binaires
	*/
	enum class ChunkType
		: uint64_t
	{
		eUnknown = MAKE_CHUNK_ID( ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ),
		eCmshFile = MAKE_CHUNK_ID( 'C', 'M', 'S', 'H', 'F', 'I', 'L', 'E' ),
		eCmshHeader = MAKE_CHUNK_ID( 'C', 'M', 'S', 'H', 'H', 'D', 'E', 'R' ),
		eCmshVersion = MAKE_CHUNK_ID( 'C', 'M', 'S', 'H', 'V', 'R', 'S', 'N' ),
		eName = MAKE_CHUNK_ID( 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' ' ),
		eMesh = MAKE_CHUNK_ID( 'M', 'E', 'S', 'H', ' ', ' ', ' ', ' ' ),
		eSkeleton = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'E', 'T', 'O', 'N' ),
		eSkeletonGlobalInverse = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'G', 'I', 'M', 'X' ),
		eSkeletonBone = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'B', 'O', 'N', 'E' ),
		eBoneParentName = MAKE_CHUNK_ID( 'B', 'O', 'N', 'E', 'P', 'A', 'R', 'T' ),
		eBoneOffsetMatrix = MAKE_CHUNK_ID( 'B', 'O', 'N', 'E', 'O', 'M', 'T', 'X' ),
		eSubmesh = MAKE_CHUNK_ID( 'S', 'U', 'B', 'M', 'E', 'S', 'H', ' ' ),
		eSubmeshTopology = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'T', 'O', 'P', 'O' ),
		eSubmeshVertexCount = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'V', 'X', 'C', 'T' ),
		eSubmeshVertex = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'V', 'R', 'T', 'X' ),
		eSubmeshBoneCount = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'B', 'N', 'C', 'T' ),
		eSubmeshBones = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'B', 'O', 'N', 'E' ),
		eSubmeshMatrixCount = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'M', 'X', 'C', 'T' ),
		eSubmeshMatrices = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'M', 'T', 'R', 'X' ),
		eSubmeshFaceCount = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'F', 'C', 'C', 'T' ),
		eSubmeshFaces = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'F', 'A', 'C', 'E' ),
		eAnimable = MAKE_CHUNK_ID( 'A', 'N', 'I', 'M', 'A', 'B', 'L', 'E' ),
		eAnimation = MAKE_CHUNK_ID( 'A', 'N', 'M', 'A', 'T', 'I', 'O', 'N' ),
		eAnimLength = MAKE_CHUNK_ID( 'A', 'N', 'M', 'L', 'E', 'N', 'G', 'T' ),
		eSkeletonAnimation = MAKE_CHUNK_ID( 'S', 'K', 'E', 'L', 'A', 'N', 'I', 'M' ),
		eMeshAnimation = MAKE_CHUNK_ID( 'M', 'E', 'S', 'H', 'A', 'N', 'I', 'M' ),
		eAnimInterpolator = MAKE_CHUNK_ID( 'A', 'N', 'M', 'I', 'N', 'T', 'E', 'R' ),
		eAnimationObject = MAKE_CHUNK_ID( 'A', 'N', 'S', 'K', 'O', 'B', 'J', 'T' ),
		eSkeletonAnimationNode = MAKE_CHUNK_ID( 'A', 'N', 'S', 'K', 'N', 'O', 'D', 'E' ),
		eSkeletonAnimationBone = MAKE_CHUNK_ID( 'A', 'N', 'S', 'K', 'B', 'O', 'N', 'E' ),
		eMovingTransform = MAKE_CHUNK_ID( 'M', 'V', 'N', 'G', 'T', 'S', 'F', 'M' ),
		eKeyframeCount = MAKE_CHUNK_ID( 'K', 'F', 'R', 'M', 'C', 'O', 'N', 'T' ),
		eKeyframes = MAKE_CHUNK_ID( 'K', 'E', 'Y', 'F', 'R', 'M', 'E', 'S' ),
		eMeshAnimationSubmeshID = MAKE_CHUNK_ID( 'M', 'H', 'A', 'N', 'S', 'H', 'I', 'D' ),
		eMeshAnimationSubmesh = MAKE_CHUNK_ID( 'M', 'S', 'H', 'A', 'N', 'S', 'M', 'H' ),
		eSubmeshAnimationBuffer = MAKE_CHUNK_ID( 'M', 'H', 'A', 'N', 'S', 'H', 'B', 'F' ),
		eSubmeshAnimationBufferSize = MAKE_CHUNK_ID( 'A', 'N', 'S', 'H', 'B', 'F', 'S', 'Z' ),
		eSubmeshAnimationBuffersCount = MAKE_CHUNK_ID( 'A', 'N', 'S', 'H', 'B', 'F', 'C', 'T' ),
		eSubmeshAnimationBuffers = MAKE_CHUNK_ID( 'A', 'N', 'S', 'H', 'B', 'U', 'F', 'S' ),
	};
	/**
	 *\~english
	 *\brief			sets given value to big endian.
	 *\param[in,out]	p_value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	p_value	La valeur.
	 */
	static inline void prepareChunkData( ChunkType & p_value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( p_value );
		}
	}
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	*/
	template< class Type >
	struct ChunkTyper;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		15/04/2013
	\~english
	\brief		Binary data chunk base class
	\~french
	\brief		Classe de base d'un chunk de données binaires
	*/
	class BinaryChunk
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API BinaryChunk();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type		The chunk type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type du chunk
		 */
		C3D_API explicit BinaryChunk( ChunkType p_type );
		/**
		 *\~english
		 *\brief		Computes the final data buffer from each one added until this call
		 *\~french
		 *\brief		Crée le tampon final à partir de tout ce qui a été ajouté jusqu'à cet appel
		 */
		C3D_API void finalise();
		/**
		 *\~english
		 *\brief		adds data to the chunk
		 *\param[in]	p_data	The data buffer
		 *\param[in]	p_size	The buffer size
		 *\~french
		 *\brief		Ajoute des données au chunk
		 *\param[in]	p_data	Le tampon de données
		 *\param[in]	p_size	La taille du tampon
		 */
		C3D_API void add( uint8_t * p_data, uint32_t p_size );
		/**
		 *\~english
		 *\brief		Retrieves data from the chunk
		 *\param[in]	p_data	The data buffer to fill
		 *\param[in]	p_size	The buffer size
		 *\~french
		 *\brief		Récupère des données du chunk
		 *\param[in]	p_data	Le tampon de données à remplir
		 *\param[in]	p_size	La taille du tampon
		 */
		C3D_API void get( uint8_t * p_data, uint32_t p_size );
		/**
		 *\~english
		 *\brief		Checks that the remaining place can hold the given size
		 *\param[in]	p_size	The size
		 *\~french
		 *\brief		Vérifie que la place restante peut contenir la taille donnée
		 *\param[in]	p_size	La taille
		 */
		C3D_API bool checkAvailable( uint32_t p_size = 0 )const;
		/**
		 *\~english
		 *\brief		Retrieves the remaining place
		 *\return		The value
		 *\~french
		 *\brief		Récupère la place restante
		 *\return		La valeur
		 */
		C3D_API uint32_t getRemaining()const;
		/**
		 *\~english
		 *\brief		Retrieves a subchunk
		 *\param[out]	p_subchunk	Receives the subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un sous chunk
		 *\param[out]	p_subchunk	Reçoit le sous chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool getSubChunk( BinaryChunk & p_subchunk );
		/**
		 *\~english
		 *\brief		Writes a subchunk into a chunk
		 *\param[in]	p_subchunk	The subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit un subchunk dans un chunk
		 *\param[in]	p_subchunk	Le subchunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool addSubChunk( BinaryChunk const & p_subchunk );
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	p_file	The file
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un fichier
		 *\param[in]	p_file	Le fichier
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool write( castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[in]	p_file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un fichier
		 *\param[in]	p_file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool read( castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		Retrieves the remaining data
		 *\return		The value
		 *\~french
		 *\brief		Récupère le tampon restant
		 *\return		La valeur
		 */
		inline uint8_t const * getRemainingData()const
		{
			return &m_data[m_index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de chunk
		 *\return		La valeur
		 */
		inline ChunkType getChunkType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille des données du chunk
		 *\return		La valeur
		 */
		inline uint32_t getDataSize()const
		{
			return uint32_t( m_data.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data
		 *\return		The value
		 *\~french
		 *\brief		Récupère les données du chunk
		 *\return		La valeur
		 */
		inline uint8_t const * getData()const
		{
			return m_data.data();
		}
		/**
		 *\~english
		 *\brief		sets the chunk's data
		 *\param[in]	p_begin	The data buffer's begin
		 *\param[in]	p_end	The data buffer's end
		 *\~french
		 *\brief		Définit les données du chunk
		 *\param[in]	p_begin	Le début du tampon de données
		 *\param[in]	p_end	La fin du tampon de données
		 */
		inline void setData( uint8_t const * p_begin, uint8_t const * p_end )
		{
			m_data.assign( p_begin, p_end );
		}
		/**
		 *\~english
		 *\brief		Retrieves the chunk data size
		 *\~french
		 *\brief		Récupère la taille des données du chunk
		 */
		void endParse()
		{
			m_index = uint32_t( m_data.size() );
		}

	private:
		template< typename T >
		inline bool doRead( T * p_values, uint32_t p_count )
		{
			auto size = p_count * uint32_t( sizeof( T ) );
			bool result{ m_index + size < m_data.size() };

			if ( result )
			{
				auto begin = reinterpret_cast< T * >( &m_data[m_index] );
				auto end = begin + p_count;
				auto value = p_values;

				for ( auto it = begin; it != end; ++it )
				{
					( *value ) = *it;
					prepareChunkData( *value );
					++value;
				}

				m_index += size;
			}

			return result;
		}

	private:
		//!\~english The chunk type	\~french Le type du chunk
		ChunkType m_type;
		//!\~english The chunk data	\~french Les données du chunk
		castor::ByteArray m_data;
		//!\~english The current index in the chunk data	\~french L'index courant dans les données du chunk
		uint32_t m_index;
		//!\~english The chunk data	\~french Les données du chunk
		std::list< castor::ByteArray > m_addedData;
	};
}

#endif
