#include "Castor3D/Binary/BinarySkeletonAnimationKeyFrame.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationKeyFrame >::doWrite( SkeletonAnimationKeyFrame const & obj )
	{
		bool result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eSkeletonAnimationKeyFrameTime, m_chunk );

		for ( auto & it : obj.m_transforms )
		{
			if ( result )
			{
				result = doWriteChunk( uint8_t( it.first->getType() ), ChunkType::eSkeletonAnimationKeyFrameObjectType, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.first->getName(), ChunkType::eSkeletonAnimationKeyFrameObjectName, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.second, ChunkType::eSkeletonAnimationKeyFrameObjectTransform, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationKeyFrame >::doParse( SkeletonAnimationKeyFrame & obj )
	{
		bool result = true;
		castor::Matrix4x4f matrix;
		double time{ 0.0 };
		castor::String name;
		uint8_t type;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonAnimationKeyFrameTime:
				result = doParseChunk( time, chunk );
				obj.doSetTimeIndex( castor::Milliseconds{ int64_t( time * 1000 ) } );
				break;

			case ChunkType::eSkeletonAnimationKeyFrameObjectType:
				result = doParseChunk( type, chunk );
				break;

			case ChunkType::eSkeletonAnimationKeyFrameObjectName:
				result = doParseChunk( name, chunk );
				break;

			case ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
				if ( m_fileVersion > Version{ 1, 3, 0 } )
				{
					result = doParseChunk( matrix, chunk );

					if ( result )
					{
						obj.addAnimationObject( *obj.getOwner()->getObject( SkeletonAnimationObjectType( type )
							, name )
							, matrix );
					}
				}
				break;

			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationKeyFrame >::doParse_v1_3( SkeletonAnimationKeyFrame & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 3, 0 } )
		{
			castor::SquareMatrix< double, 4 > matrix;
			castor::String name;
			uint8_t type;
			BinaryChunk chunk;

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
				case ChunkType::eSkeletonAnimationKeyFrameObjectType:
					result = doParseChunk( type, chunk );
					break;

				case ChunkType::eSkeletonAnimationKeyFrameObjectName:
					result = doParseChunk( name, chunk );
					break;

				case ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
					result = doParseChunk( matrix, chunk );

					if ( result )
					{
						obj.addAnimationObject( *obj.getOwner()->getObject( SkeletonAnimationObjectType( type )
							, name )
							, castor::Matrix4x4f{ matrix } );
					}

					break;

				default:
					break;
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
