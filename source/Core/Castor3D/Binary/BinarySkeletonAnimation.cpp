#include "BinarySkeletonAnimation.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Animation/Skeleton/SkeletonAnimationKeyFrame.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"
#include "Binary/BinarySkeletonAnimationBone.hpp"
#include "Binary/BinarySkeletonAnimationNode.hpp"
#include "Binary/BinarySkeletonAnimationKeyFrame.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimation >::doWrite( SkeletonAnimation const & obj )
	{
		bool result = true;

		for ( auto moving : obj.m_arrayMoving )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				result &= BinaryWriter< SkeletonAnimationNode >{}.write( *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				result &= BinaryWriter< SkeletonAnimationBone >{}.write( *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_chunk );
				break;
			}
		}

		for ( auto & keyframe : obj )
		{
			result &= BinaryWriter< SkeletonAnimationKeyFrame >{}.write( static_cast< SkeletonAnimationKeyFrame const & >( *keyframe ), m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimation >::doParse( SkeletonAnimation & obj )
	{
		bool result = true;
		SkeletonAnimationNodeSPtr node;
		SkeletonAnimationObjectSPtr object;
		SkeletonAnimationBoneSPtr bone;
		SkeletonAnimationKeyFrameUPtr keyFrame;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonAnimationNode:
				node = std::make_shared< SkeletonAnimationNode >( obj );
				result = createBinaryParser< SkeletonAnimationNode >().parse( *node, chunk );

				if ( result )
				{
					obj.addObject( node, nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = std::make_shared< SkeletonAnimationBone >( obj );
				result = createBinaryParser< SkeletonAnimationBone >().parse( *bone, chunk );

				if ( result )
				{
					obj.addObject( bone, nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationKeyFrame:
				keyFrame = std::make_unique< SkeletonAnimationKeyFrame >( obj );
				result = createBinaryParser< SkeletonAnimationKeyFrame >().parse( *keyFrame, chunk );

				if ( result )
				{
					obj.addKeyFrame( std::move( keyFrame ) );
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
