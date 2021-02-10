/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryChunk_H___
#define ___C3D_BinaryChunk_H___

#include "BinaryModule.hpp"

#include "Castor3D/Binary/ChunkData.hpp"

#include <cstdint>
#include <cstring>

namespace castor3d
{
	inline constexpr uint32_t makeCmshVersion( uint32_t maj
		, uint32_t min
		, uint32_t rev )
	{
		return ( ( maj << 24 )
			| ( min << 16 )
			| ( rev << 0 ) );
	}

	inline constexpr uint32_t getCmshMajor( uint32_t version )
	{
		return ( version >> 24 );
	}

	inline constexpr uint32_t getCmshMinor( uint32_t version )
	{
		return ( ( version >> 16 ) & uint32_t( 0xff ) );
	}

	inline constexpr uint32_t getCmshRevision( uint32_t version )
	{
		return ( version & uint32_t( 0xff ) );
	}

	//!\~english	The current format version number.
	//!\~french		La version actuelle du format.
	uint32_t constexpr CurrentCmshVersion = makeCmshVersion( 0x01u, 0x05u, 0x0000u );
	//!\~english	A define to ease the declaration of a chunk id.
	//!\~french		Un define pour faciliter la déclaration d'un id de chunk.
	uint64_t constexpr makeChunkID( char a, char b, char c, char d
		, char e, char f, char g, char h )
	{
		return ( ( uint64_t( a ) << 56 )
			| ( uint64_t( b ) << 48 )
			| ( uint64_t( c ) << 40 )
			| ( uint64_t( d ) << 32 )
			| ( uint64_t( e ) << 24 )
			| ( uint64_t( f ) << 16 )
			| ( uint64_t( g ) << 8 )
			| ( uint64_t( h ) << 0 ) );
	}
	/**
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
		eUnknown = makeChunkID( ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ),
		eCmshFile = makeChunkID( 'C', 'M', 'S', 'H', 'F', 'I', 'L', 'E' ),
		eCmshHeader = makeChunkID( 'C', 'M', 'S', 'H', 'H', 'D', 'E', 'R' ),
		eCmshVersion = makeChunkID( 'C', 'M', 'S', 'H', 'V', 'R', 'S', 'N' ),
		eName = makeChunkID( 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' ' ),
		eMesh = makeChunkID( 'M', 'E', 'S', 'H', ' ', ' ', ' ', ' ' ),
		eSkeleton = makeChunkID( 'S', 'K', 'E', 'L', 'E', 'T', 'O', 'N' ),
		eSkeletonGlobalInverse = makeChunkID( 'S', 'K', 'E', 'L', 'G', 'I', 'M', 'X' ),
		eSkeletonBone = makeChunkID( 'S', 'K', 'E', 'L', 'B', 'O', 'N', 'E' ),
		eBoneParentName = makeChunkID( 'B', 'O', 'N', 'E', 'P', 'A', 'R', 'T' ),
		eBoneOffsetMatrix = makeChunkID( 'B', 'O', 'N', 'E', 'O', 'M', 'T', 'X' ),
		eSubmesh = makeChunkID( 'S', 'U', 'B', 'M', 'E', 'S', 'H', ' ' ),
		eSubmeshTopology = makeChunkID( 'S', 'M', 'S', 'H', 'T', 'O', 'P', 'O' ),
		eSubmeshVertexCount = makeChunkID( 'S', 'M', 'S', 'H', 'V', 'X', 'C', 'T' ),
		eSubmeshVertex = makeChunkID( 'S', 'M', 'S', 'H', 'V', 'R', 'T', 'X' ),
		eSubmeshBoneCount = makeChunkID( 'S', 'M', 'S', 'H', 'B', 'N', 'C', 'T' ),
		eSubmeshBones = makeChunkID( 'S', 'M', 'S', 'H', 'B', 'O', 'N', 'E' ),
		eSubmeshMatrixCount = makeChunkID( 'S', 'M', 'S', 'H', 'M', 'X', 'C', 'T' ),
		eSubmeshMatrices = makeChunkID( 'S', 'M', 'S', 'H', 'M', 'T', 'R', 'X' ),
		eSubmeshFaceCount [[deprecated( "Use eSubmeshIndexCount" )]] = makeChunkID( 'S', 'M', 'S', 'H', 'F', 'C', 'C', 'T' ),
		eSubmeshFaces [[deprecated( "Use eSubmeshIndices" )]] = makeChunkID( 'S', 'M', 'S', 'H', 'F', 'A', 'C', 'E' ),
		eAnimable = makeChunkID( 'A', 'N', 'I', 'M', 'A', 'B', 'L', 'E' ),
		eAnimation = makeChunkID( 'A', 'N', 'M', 'A', 'T', 'I', 'O', 'N' ),
		eAnimLength = makeChunkID( 'A', 'N', 'M', 'L', 'E', 'N', 'G', 'T' ),
		eSkeletonAnimation = makeChunkID( 'S', 'K', 'E', 'L', 'A', 'N', 'I', 'M' ),
		eMeshAnimation = makeChunkID( 'M', 'E', 'S', 'H', 'A', 'N', 'I', 'M' ),
		eAnimInterpolator = makeChunkID( 'A', 'N', 'M', 'I', 'N', 'T', 'E', 'R' ),
		eAnimationObject = makeChunkID( 'A', 'N', 'S', 'K', 'O', 'B', 'J', 'T' ),
		eSkeletonAnimationNode = makeChunkID( 'A', 'N', 'S', 'K', 'N', 'O', 'D', 'E' ),
		eSkeletonAnimationBone = makeChunkID( 'A', 'N', 'S', 'K', 'B', 'O', 'N', 'E' ),
		eMovingTransform = makeChunkID( 'M', 'V', 'N', 'G', 'T', 'S', 'F', 'M' ),
		eKeyframeCount [[deprecated]] = makeChunkID( 'K', 'F', 'R', 'M', 'C', 'O', 'N', 'T' ),
		eKeyframes [[deprecated]] = makeChunkID( 'K', 'E', 'Y', 'F', 'R', 'M', 'E', 'S' ),
		eBonesComponent = makeChunkID( 'B', 'O', 'N', 'E', 'C', 'O', 'M', 'P' ),
		// Version 1.2
		eMeshAnimationKeyFrame = makeChunkID( 'M', 'S', 'A', 'N', 'K', 'F', 'R', 'M' ),
		eMeshAnimationKeyFrameTime = makeChunkID( 'M', 'S', 'A', 'N', 'K', 'F', 'T', 'M' ),
		eMeshAnimationKeyFrameSubmeshID = makeChunkID( 'M', 'S', 'A', 'N', 'K', 'F', 'I', 'D' ),
		eMeshAnimationKeyFrameBufferSize = makeChunkID( 'M', 'H', 'A', 'N', 'K', 'F', 'S', 'Z' ),
		eMeshAnimationKeyFrameBufferData = makeChunkID( 'M', 'H', 'A', 'N', 'K', 'F', 'D', 'T' ),
		eSkeletonAnimationKeyFrame = makeChunkID( 'S', 'K', 'A', 'N', 'K', 'F', 'R', 'M' ),
		eSkeletonAnimationKeyFrameTime = makeChunkID( 'S', 'K', 'A', 'N', 'K', 'F', 'T', 'M' ),
		eSkeletonAnimationKeyFrameObjectType = makeChunkID( 'S', 'K', 'A', 'N', 'K', 'F', 'O', 'Y' ),
		eSkeletonAnimationKeyFrameObjectName = makeChunkID( 'S', 'K', 'A', 'N', 'K', 'F', 'O', 'N' ),
		eSkeletonAnimationKeyFrameObjectTransform = makeChunkID( 'S', 'K', 'A', 'N', 'K', 'F', 'O', 'T' ),
		// Version 1.4
		eSubmeshIndexComponentCount = makeChunkID( 'S', 'M', 'F', 'C', 'C', 'P', 'C', 'T' ),
		eSubmeshIndexCount = makeChunkID( 'S', 'M', 'S', 'H', 'I', 'C', 'C', 'T' ),
		eSubmeshIndices = makeChunkID( 'S', 'M', 'S', 'H', 'I', 'D', 'C', 'S' ),
	};
	/**
	 *\~english
	 *\brief			Sets given value to big endian.
	 *\param[in,out]	value	The value.
	 *\~french
	 *\brief			Met la valeur donnée en big endian.
	 *\param[in,out]	value	La valeur.
	 */
	static inline void prepareChunkData( ChunkType & value )
	{
		if ( !castor::isBigEndian() )
		{
			castor::switchEndianness( value );
		}
	}

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
		 *\param[in]	type		The chunk type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	type		Le type du chunk
		 */
		C3D_API explicit BinaryChunk( ChunkType type );
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
		 *\param[in]	data	The data buffer
		 *\param[in]	size	The buffer size
		 *\~french
		 *\brief		Ajoute des données au chunk
		 *\param[in]	data	Le tampon de données
		 *\param[in]	size	La taille du tampon
		 */
		C3D_API void add( uint8_t * data, uint32_t size );
		/**
		 *\~english
		 *\brief		Retrieves data from the chunk
		 *\param[in]	data	The data buffer to fill
		 *\param[in]	size	The buffer size
		 *\~french
		 *\brief		Récupère des données du chunk
		 *\param[in]	data	Le tampon de données à remplir
		 *\param[in]	size	La taille du tampon
		 */
		C3D_API void get( uint8_t * data, uint32_t size );
		/**
		 *\~english
		 *\brief		Checks that the remaining place can hold the given size
		 *\param[in]	size	The size
		 *\~french
		 *\brief		Vérifie que la place restante peut contenir la taille donnée
		 *\param[in]	size	La taille
		 */
		C3D_API bool checkAvailable( uint32_t size = 0 )const;
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
		 *\param[out]	subchunk	Receives the subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Récupère un sous chunk
		 *\param[out]	subchunk	Reçoit le sous chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool getSubChunk( BinaryChunk & subchunk );
		/**
		 *\~english
		 *\brief		Writes a subchunk into a chunk
		 *\param[in]	subchunk	The subchunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Ecrit un subchunk dans un chunk
		 *\param[in]	subchunk	Le subchunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool addSubChunk( BinaryChunk const & subchunk );
		/**
		 *\~english
		 *\brief		To chunk writer function
		 *\param[in]	file	The file
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction d'écriture dans un fichier
		 *\param[in]	file	Le fichier
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool write( castor::BinaryFile & file );
		/**
		 *\~english
		 *\brief		From file reader function
		 *\param[in]	file	The file containing the chunk
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction de lecture à partir d'un fichier
		 *\param[in]	file	Le fichier qui contient le chunk
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool read( castor::BinaryFile & file );
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
		 *\brief		Sets the chunk's data
		 *\param[in]	begin	The data buffer's begin
		 *\param[in]	end		The data buffer's end
		 *\~french
		 *\brief		Définit les données du chunk
		 *\param[in]	begin	Le début du tampon de données
		 *\param[in]	end		La fin du tampon de données
		 */
		inline void setData( uint8_t const * begin
			, uint8_t const * end )
		{
			m_data.assign( begin, end );
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
		/**
		 *\~english
		 *\brief		Resets the chunk, to be able to reparse it.
		 *\~french
		 *\brief		Réinitialise le chunk, afin de pouvoir le reparser.
		 */
		void resetParse()
		{
			m_index = 0u;
		}

	private:
		C3D_API void binaryError( std::string_view view );

	private:
		template< typename T >
		inline bool doRead( T * values
			, uint32_t count )
		{
			auto size = count * uint32_t( sizeof( T ) );
			bool result{ m_index + size < m_data.size() };

			if ( result )
			{
				auto begin = reinterpret_cast< T * >( &m_data[m_index] );
				auto end = begin + count;
				auto value = values;

				for ( auto it = begin; it != end; ++it )
				{
					( *value ) = *it;
					prepareChunkData( *value );
					++value;
				}

				m_index += size;
			}
			else
			{
				binaryError( "Not enough data in chunk" );
			}

			return result;
		}

	private:
		ChunkType m_type;
		castor::ByteArray m_data;
		uint32_t m_index;
		std::list< castor::ByteArray > m_addedData;
	};
}

#endif
