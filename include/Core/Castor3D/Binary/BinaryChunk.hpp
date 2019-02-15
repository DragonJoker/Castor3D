/*
See LICENSE file in root folder
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
#	define	MAKE_CMSH_VERSION( maj, min, rev )\
	( ( uint32_t( maj ) << 24 ) \
	| ( uint32_t( min ) << 16 ) \
	| ( uint32_t( rev ) <<  0 ) )
	//!\~english	Macro to get the major number from a version number.
	//!\~french		Macro pour créer le numéro majeur depuis un numéro de version.
#	define	CMSH_VERSION_MAJOR( version )\
	( uint32_t( version ) >> 24 )
	//!\~english	Macro to get the minor number from a version number.
	//!\~french		Macro pour créer le numéro mineur depuis un numéro de version.
#	define	CMSH_VERSION_MINOR( version )\
	( ( uint32_t( version ) >> 16 ) & uint32_t( 0xFF ) )
	//!\~english	Macro to get the revision number from a version number.
	//!\~french		Macro pour créer le numéro de révision depuis un numéro de version.
#	define	CMSH_VERSION_REVISION( version )\
	( ( uint32_t( version ) >>  0 ) & uint32_t( 0xFF ) )
	//!\~english	The current format version number.
	//!\~french		La version actuelle du format.
	uint32_t const CMSH_VERSION = MAKE_CMSH_VERSION( 0x01, 0x04, 0x0000 );
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
#	define MAKE_DEPRECATED_CHUNK_ID( a, b, c, d, e, f, g, h )\
	MAKE_CHUNK_ID( a, b, c, d, e, f, g, h )
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
		eSkeleton = MAKE_DEPRECATED_CHUNK_ID( 'S', 'K', 'E', 'L', 'E', 'T', 'O', 'N' ),
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
		eSubmeshFaceCount = MAKE_DEPRECATED_CHUNK_ID( 'S', 'M', 'S', 'H', 'F', 'C', 'C', 'T' ),
		eSubmeshFaces = MAKE_DEPRECATED_CHUNK_ID( 'S', 'M', 'S', 'H', 'F', 'A', 'C', 'E' ),
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
		eKeyframeCount = MAKE_DEPRECATED_CHUNK_ID( 'K', 'F', 'R', 'M', 'C', 'O', 'N', 'T' ),
		eKeyframes = MAKE_DEPRECATED_CHUNK_ID( 'K', 'E', 'Y', 'F', 'R', 'M', 'E', 'S' ),
		eBonesComponent = MAKE_CHUNK_ID( 'B', 'O', 'N', 'E', 'C', 'O', 'M', 'P' ),
		// Version 1.2
		eMeshAnimationKeyFrame = MAKE_CHUNK_ID( 'M', 'S', 'A', 'N', 'K', 'F', 'R', 'M' ),
		eMeshAnimationKeyFrameTime = MAKE_CHUNK_ID( 'M', 'S', 'A', 'N', 'K', 'F', 'T', 'M' ),
		eMeshAnimationKeyFrameSubmeshID = MAKE_CHUNK_ID( 'M', 'S', 'A', 'N', 'K', 'F', 'I', 'D' ),
		eMeshAnimationKeyFrameBufferSize = MAKE_CHUNK_ID( 'M', 'H', 'A', 'N', 'K', 'F', 'S', 'Z' ),
		eMeshAnimationKeyFrameBufferData = MAKE_CHUNK_ID( 'M', 'H', 'A', 'N', 'K', 'F', 'D', 'T' ),
		eSkeletonAnimationKeyFrame = MAKE_CHUNK_ID( 'S', 'K', 'A', 'N', 'K', 'F', 'R', 'M' ),
		eSkeletonAnimationKeyFrameTime = MAKE_CHUNK_ID( 'S', 'K', 'A', 'N', 'K', 'F', 'T', 'M' ),
		eSkeletonAnimationKeyFrameObjectType = MAKE_CHUNK_ID( 'S', 'K', 'A', 'N', 'K', 'F', 'O', 'Y' ),
		eSkeletonAnimationKeyFrameObjectName = MAKE_CHUNK_ID( 'S', 'K', 'A', 'N', 'K', 'F', 'O', 'N' ),
		eSkeletonAnimationKeyFrameObjectTransform = MAKE_CHUNK_ID( 'S', 'K', 'A', 'N', 'K', 'F', 'O', 'T' ),
		// Version 1.4
		eSubmeshIndexComponentCount = MAKE_CHUNK_ID( 'S', 'M', 'F', 'C', 'C', 'P', 'C', 'T' ),
		eSubmeshIndexCount = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'I', 'C', 'C', 'T' ),
		eSubmeshIndices = MAKE_CHUNK_ID( 'S', 'M', 'S', 'H', 'I', 'D', 'C', 'S' ),
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

			return result;
		}

	private:
		//!\~english	The chunk type.
		//!\~french		Le type du chunk.
		ChunkType m_type;
		//!\~english	The chunk data.
		//!\~french		Les données du chunk.
		castor::ByteArray m_data;
		//!\~english	The current index in the chunk data.
		//!\~french		L'index courant dans les données du chunk.
		uint32_t m_index;
		//!\~english	The chunk data.
		//!\~french		Les données du chunk.
		std::list< castor::ByteArray > m_addedData;
	};
}

#endif
