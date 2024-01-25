#include "Castor3D/Binary/BinaryChunk.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>

#include <numeric>

namespace castor3d
{
	//*********************************************************************************************

	namespace binchunk
	{
		static bool isValidType( ChunkType v )
		{
			switch ( v )
			{
			case castor3d::ChunkType::eUnknown:
			case castor3d::ChunkType::eCmshFile:
			case castor3d::ChunkType::eCmshHeader:
			case castor3d::ChunkType::eCmshVersion:
			case castor3d::ChunkType::eName:
			case castor3d::ChunkType::eMesh:
			case castor3d::ChunkType::eSkeleton:
			case castor3d::ChunkType::eSkeletonGlobalInverse:
			case castor3d::ChunkType::eSkeletonBone:
			case castor3d::ChunkType::eSkeletonNode:
			case castor3d::ChunkType::eBoneParentName:
			case castor3d::ChunkType::eBoneOffsetMatrix:
			case castor3d::ChunkType::eSubmesh:
			case castor3d::ChunkType::eSubmeshTopology:
			case castor3d::ChunkType::eSubmeshVertexCount:
			case castor3d::ChunkType::eSubmeshBoneCount:
			case castor3d::ChunkType::eSubmeshBones:
			case castor3d::ChunkType::eSubmeshMatrixCount:
			case castor3d::ChunkType::eSubmeshMatrices:
			case castor3d::ChunkType::eAnimable:
			case castor3d::ChunkType::eAnimation:
			case castor3d::ChunkType::eAnimLength:
			case castor3d::ChunkType::eSkeletonAnimation:
			case castor3d::ChunkType::eMeshAnimation:
			case castor3d::ChunkType::eAnimInterpolator:
			case castor3d::ChunkType::eAnimationObject:
			case castor3d::ChunkType::eSkeletonAnimationNode:
			case castor3d::ChunkType::eSkeletonAnimationBone:
			case castor3d::ChunkType::eMovingTransform:
			case castor3d::ChunkType::eBonesComponent:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrame:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrameTime:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrameObjectType:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrameObjectName:
			case castor3d::ChunkType::eSubmeshIndexComponentCount:
			case castor3d::ChunkType::eSubmeshIndexCount:
			case castor3d::ChunkType::eSubmeshIndices:
			case castor3d::ChunkType::eSceneNodeAnimation:
			case castor3d::ChunkType::eSceneNodeAnimationKeyFrame:
			case castor3d::ChunkType::eSceneNodeAnimationKeyFrameTime:
			case castor3d::ChunkType::eSceneNodeAnimationKeyFrameTranslate:
			case castor3d::ChunkType::eSceneNodeAnimationKeyFrameRotate:
			case castor3d::ChunkType::eSceneNodeAnimationKeyFrameScale:
			case castor3d::ChunkType::eBoneId:
			case castor3d::ChunkType::eSubmeshPositions:
			case castor3d::ChunkType::eSubmeshNormals:
			case castor3d::ChunkType::eSubmeshTexcoords0:
			case castor3d::ChunkType::eSubmeshTexcoords1:
			case castor3d::ChunkType::eSubmeshTexcoords2:
			case castor3d::ChunkType::eSubmeshTexcoords3:
			case castor3d::ChunkType::eSubmeshColours:
			case castor3d::ChunkType::eMorphComponent:
			case castor3d::ChunkType::eMorphTargetBufferSize:
			case castor3d::ChunkType::eMorphTargetPositions:
			case castor3d::ChunkType::eMorphTargetNormals:
			case castor3d::ChunkType::eMorphTargetTexcoords0:
			case castor3d::ChunkType::eMorphTargetTexcoords1:
			case castor3d::ChunkType::eMorphTargetTexcoords2:
			case castor3d::ChunkType::eMorphTargetTexcoords3:
			case castor3d::ChunkType::eMorphTargetColours:
			case castor3d::ChunkType::eMeshMorphTarget:
			case castor3d::ChunkType::eMeshMorphTargetTime:
			case castor3d::ChunkType::eMeshMorphTargetSubmeshID:
			case castor3d::ChunkType::eMeshMorphTargetWeights:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrameObjectTranslate:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrameObjectRotate:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrameObjectScale:
			case castor3d::ChunkType::eSubmeshTangentsMikkt:
			case castor3d::ChunkType::eMorphTargetTangentsMikkt:
			case castor3d::ChunkType::eSubmeshBitangents:
			case castor3d::ChunkType::eMorphTargetBitangents:
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			case castor3d::ChunkType::eSubmeshTangents:
			case castor3d::ChunkType::eMorphTargetTangents:
			case castor3d::ChunkType::eKeyframeCount:
			case castor3d::ChunkType::eKeyframes:
			case castor3d::ChunkType::eSubmeshFaceCount:
			case castor3d::ChunkType::eSubmeshFaces:
			case castor3d::ChunkType::eSubmeshVertex:
			case castor3d::ChunkType::eMeshAnimationKeyFrame:
			case castor3d::ChunkType::eMeshAnimationKeyFrameTime:
			case castor3d::ChunkType::eMeshAnimationKeyFrameBufferData:
			case castor3d::ChunkType::eMeshAnimationKeyFrameSubmeshID:
			case castor3d::ChunkType::eMeshAnimationKeyFrameBufferSize:
			case castor3d::ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
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
			, [&]( uint32_t value, castor::ByteArray const & array )
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

	void BinaryChunk::add( castor::ByteArray data )
	{
		m_addedData.push_back( castor::move( data ) );
	}

	void BinaryChunk::add( uint8_t * data, uint32_t size )
	{
		add( castor::ByteArray( data, data + size ) );
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
		castor::ByteArray buffer;
		buffer.reserve( sizeof( uint32_t ) + sizeof( ChunkType ) + size );

		// Write subchunk type,
		auto type = castor::systemEndianToLittleEndian( subchunk.m_type );
		auto data = ByteCPtr( &type );
		buffer.insert( buffer.end(), data, data + sizeof( ChunkType ) );
		// Then its size,
		castor::systemEndianToLittleEndian( size );
		data = ByteCPtr( &size );
		buffer.insert( buffer.end(), data, data + sizeof( uint32_t ) );
		// And eventually its data.
		buffer.insert( buffer.end(), subchunk.m_data.begin(), subchunk.m_data.end() );

		// Now add it to this chunk
		add( castor::move( buffer ) );

		return true;
	}

	bool BinaryChunk::write( castor::BinaryFile & file )
	{
		auto type = castor::systemEndianToLittleEndian( getChunkType() );
		auto result = file.write( type ) == sizeof( ChunkType );

		if ( result )
		{
			finalise();
			auto size = castor::systemEndianToLittleEndian( getDataSize() );
			result = file.write( size ) == sizeof( uint32_t );
		}

		if ( result )
		{
			result = file.writeArray( m_data.data(), m_data.size() ) == m_data.size();
		}

		return result;
	}

	bool BinaryChunk::read( castor::BinaryFile & file )
	{
		uint32_t size = 0;
		bool result = file.read( m_type ) == sizeof( ChunkType );

		if ( result )
		{
			m_isLittleEndian = binchunk::isValidType( m_type );

			if ( !m_isLittleEndian )
			{
				castor::switchEndianness( m_type );
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

	void BinaryChunk::binaryError( castor::StringView view )const
	{
		log::error << view;
	}

	//*********************************************************************************************
}
