#include "BinarySkeletonAnimationKeyFrame.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"
#include "Animation/Skeleton/SkeletonAnimationKeyFrame.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationKeyFrame >::doWrite( SkeletonAnimationKeyFrame const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eSkeletonAnimationKeyFrameTime, m_chunk );
		}

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
		Matrix4x4f matrix;
		double time{ 0.0 };
		String name;
		uint8_t type;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonAnimationKeyFrameTime:
				result = doParseChunk( time, chunk );
				obj.doSetTimeIndex( Milliseconds{ int64_t( time * 1000 ) } );
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
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationKeyFrame >::doParse_v1_3( SkeletonAnimationKeyFrame & obj )
	{
		bool result = true;

		if ( m_fileVersion == Version{ 1, 3, 0 } )
		{
			SquareMatrix< double, 4 > matrix;
			String name;
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
							, Matrix4x4r{ matrix } );
					}

					break;
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
