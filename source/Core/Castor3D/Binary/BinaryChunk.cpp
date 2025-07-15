#include "Castor3D/Binary/BinaryChunk.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>

#include <numeric>

namespace c3d
{
	//*********************************************************************************************

	namespace binchunk
	{
		static bool isValidType( ChunkType v )
		{
			switch ( v )
			{
			case ChunkType::eUnknown:
			case ChunkType::eCmshFile:
			case ChunkType::eCmshHeader:
			case ChunkType::eCmshVersion:
			case ChunkType::eName:
			case ChunkType::eMesh:
			case ChunkType::eSkeleton:
			case ChunkType::eSkeletonGlobalInverse:
			case ChunkType::eSkeletonBone:
			case ChunkType::eSkeletonNode:
			case ChunkType::eBoneParentName:
			case ChunkType::eBoneOffsetMatrix:
			case ChunkType::eSubmesh:
			case ChunkType::eSubmeshTopology:
			case ChunkType::eSubmeshVertexCount:
			case ChunkType::eSubmeshBoneCount:
			case ChunkType::eSubmeshBones:
			case ChunkType::eSubmeshMatrixCount:
			case ChunkType::eSubmeshMatrices:
			case ChunkType::eAnimable:
			case ChunkType::eAnimation:
			case ChunkType::eAnimLength:
			case ChunkType::eSkeletonAnimation:
			case ChunkType::eMeshAnimation:
			case ChunkType::eAnimInterpolator:
			case ChunkType::eAnimationObject:
			case ChunkType::eSkeletonAnimationNode:
			case ChunkType::eSkeletonAnimationBone:
			case ChunkType::eMovingTransform:
			case ChunkType::eBonesComponent:
			case ChunkType::eSkeletonAnimationKeyFrame:
			case ChunkType::eSkeletonAnimationKeyFrameTime:
			case ChunkType::eSkeletonAnimationKeyFrameObjectType:
			case ChunkType::eSkeletonAnimationKeyFrameObjectName:
			case ChunkType::eSubmeshIndexComponentCount:
			case ChunkType::eSubmeshIndexCount:
			case ChunkType::eSubmeshIndices:
			case ChunkType::eSceneNodeAnimation:
			case ChunkType::eSceneNodeAnimationKeyFrame:
			case ChunkType::eSceneNodeAnimationKeyFrameTime:
			case ChunkType::eSceneNodeAnimationKeyFrameTranslate:
			case ChunkType::eSceneNodeAnimationKeyFrameRotate:
			case ChunkType::eSceneNodeAnimationKeyFrameScale:
			case ChunkType::eBoneId:
			case ChunkType::eSubmeshPositions:
			case ChunkType::eSubmeshNormals:
			case ChunkType::eSubmeshTexcoords0:
			case ChunkType::eSubmeshTexcoords1:
			case ChunkType::eSubmeshTexcoords2:
			case ChunkType::eSubmeshTexcoords3:
			case ChunkType::eSubmeshColours:
			case ChunkType::eMorphComponent:
			case ChunkType::eMorphTargetBufferSize:
			case ChunkType::eMorphTargetPositions:
			case ChunkType::eMorphTargetNormals:
			case ChunkType::eMorphTargetTexcoords0:
			case ChunkType::eMorphTargetTexcoords1:
			case ChunkType::eMorphTargetTexcoords2:
			case ChunkType::eMorphTargetTexcoords3:
			case ChunkType::eMorphTargetColours:
			case ChunkType::eMeshMorphTarget:
			case ChunkType::eMeshMorphTargetTime:
			case ChunkType::eMeshMorphTargetSubmeshID:
			case ChunkType::eMeshMorphTargetWeights:
			case ChunkType::eSkeletonAnimationKeyFrameObjectTranslate:
			case ChunkType::eSkeletonAnimationKeyFrameObjectRotate:
			case ChunkType::eSkeletonAnimationKeyFrameObjectScale:
			case ChunkType::eSubmeshTangentsMikkt:
			case ChunkType::eMorphTargetTangentsMikkt:
			case ChunkType::eSubmeshBitangents:
			case ChunkType::eMorphTargetBitangents:
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			case ChunkType::eSubmeshTangents:
			case ChunkType::eMorphTargetTangents:
			case ChunkType::eKeyframeCount:
			case ChunkType::eKeyframes:
			case ChunkType::eSubmeshFaceCount:
			case ChunkType::eSubmeshFaces:
			case ChunkType::eSubmeshVertex:
			case ChunkType::eMeshAnimationKeyFrame:
			case ChunkType::eMeshAnimationKeyFrameTime:
			case ChunkType::eMeshAnimationKeyFrameBufferData:
			case ChunkType::eMeshAnimationKeyFrameSubmeshID:
			case ChunkType::eMeshAnimationKeyFrameBufferSize:
			case ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
#pragma GCC diagnostic pop
#pragma warning( pop )
				return true;
			}

			return false;
		}
	}

	//*********************************************************************************************

	bool isLittleEndian( BinaryChunk const & chunk )
	{
		return chunk.isLittleEndian();
	}

	//*********************************************************************************************

	BinaryChunk::BinaryChunk( bool isLittleEndian )
		: m_type{ ChunkType::eUnknown }
		, m_isLittleEndian{ isLittleEndian }
	{
	}

	BinaryChunk::BinaryChunk( ChunkType type )
		: m_type{ type }
	{
	}

	void BinaryChunk::finalise()
	{
		uint32_t size = std::accumulate( m_addedData.begin()
			, m_addedData.end()
			, uint32_t{}
			, [&]( uint32_t value, ByteArray const & array )
			{
				return value + uint32_t( array.size() );
			} );
		m_data.resize( size );
		size_t index = 0;

		for ( auto const & array : m_addedData )
		{
			std::memcpy( &m_data[index], array.data(), array.size() );
			index += array.size();
		}
	}

	void BinaryChunk::add( ByteArray data )
	{
		m_addedData.push_back( c3d::move( data ) );
	}

	void BinaryChunk::add( uint8_t * data, uint32_t size )
	{
		add( ByteArray( data, data + size ) );
	}

	void BinaryChunk::get( uint8_t * data, uint32_t size )
	{
		std::memcpy( data, &m_data[m_index], size );
		m_index += size;
	}

	bool BinaryChunk::checkAvailable( uint32_t size )const
	{
		return size_t( m_index ) + size <= m_data.size();
	}

	uint32_t BinaryChunk::getRemaining()const
	{
		return uint32_t( m_data.size() - m_index );
	}

	bool BinaryChunk::getSubChunk( BinaryChunk & chunkDst )
	{
		// First we retrieve the chunk type
		BinaryChunk subchunk{ m_isLittleEndian };
		bool result = doRead( &subchunk.m_type, 1 );
		uint32_t size = 0;

		if ( result )
		{
			// Then the chunk data size
			result = doRead( &size, 1 );
		}

		if ( result )
		{
			result = size_t( m_index ) + size <= m_data.size();
		}

		if ( result )
		{
			// Eventually we retrieve the chunk data
			subchunk.m_data.insert( subchunk.m_data.end(), m_data.begin() + m_index, m_data.begin() + m_index + size );
			subchunk.m_index = 0;
			m_index += size;
			chunkDst = subchunk;
		}

		return result;
	}

	bool BinaryChunk::addSubChunk( BinaryChunk const & subchunk )
	{
		auto size = uint32_t( subchunk.m_data.size() );
		ByteArray buffer;
		buffer.reserve( sizeof( uint32_t ) + sizeof( ChunkType ) + size );

		// Write subchunk type,
		auto type = systemEndianToLittleEndian( subchunk.m_type );
		auto data = ByteCPtr( &type );
		buffer.insert( buffer.end(), data, data + sizeof( ChunkType ) );
		// Then its size,
		systemEndianToLittleEndian( size );
		data = ByteCPtr( &size );
		buffer.insert( buffer.end(), data, data + sizeof( uint32_t ) );
		// And eventually its data.
		buffer.insert( buffer.end(), subchunk.m_data.begin(), subchunk.m_data.end() );

		// Now add it to this chunk
		add( c3d::move( buffer ) );

		return true;
	}

	bool BinaryChunk::write( BinaryFile & file )
	{
		auto type = systemEndianToLittleEndian( getChunkType() );
		auto result = file.write( type ) == sizeof( ChunkType );

		if ( result )
		{
			finalise();
			auto size = systemEndianToLittleEndian( getDataSize() );
			result = file.write( size ) == sizeof( uint32_t );
		}

		if ( result )
		{
			result = file.writeArray( m_data.data(), m_data.size() ) == m_data.size();
		}

		return result;
	}

	bool BinaryChunk::read( BinaryFile & file )
	{
		uint32_t size = 0;
		bool result = file.read( m_type ) == sizeof( ChunkType );

		if ( result )
		{
			m_isLittleEndian = binchunk::isValidType( m_type );

			if ( !m_isLittleEndian )
			{
				switchEndianness( m_type );
				result = binchunk::isValidType( m_type );
			}
		}

		if ( result )
		{
			result = file.read( size ) == sizeof( uint32_t );
			chunkEndianToSystemEndian( *this, size );
		}

		if ( result )
		{
			m_data.resize( size );
			result = file.readArray( m_data.data(), m_data.size() ) == m_data.size();
		}

		return result;
	}

	void BinaryChunk::binaryError( String view )const
	{
		log::error << view;
	}

	//*********************************************************************************************
}
