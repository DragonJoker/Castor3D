#include "Castor3D/Binary/BinarySkeletonAnimation.hpp"

#include "Castor3D/Binary/BinarySkeletonAnimationBone.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationNode.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimation >::doWrite( SkeletonAnimation const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		for ( auto moving : obj.m_arrayMoving )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				result = result && BinaryWriter< SkeletonAnimationNode >{}.write( *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				result = result && BinaryWriter< SkeletonAnimationBone >{}.write( *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_chunk );
				break;

			default:
				break;
			}
		}

		for ( auto & keyframe : obj )
		{
			result = result && BinaryWriter< SkeletonAnimationKeyFrame >{}.write( static_cast< SkeletonAnimationKeyFrame const & >( *keyframe ), m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< SkeletonAnimation >::Name = cuT( "SkeletonAnimation" );

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
			case ChunkType::eName:
				result = doParseChunk( name, chunk );
				checkError( result, "Couldn't parse name." );

				if ( result )
				{
					obj.m_name = name;
				}

				break;

			case ChunkType::eSkeletonAnimationNode:
				node = std::make_shared< SkeletonAnimationNode >( obj );
				result = createBinaryParser< SkeletonAnimationNode >().parse( *node, chunk );
				checkError( result, "Couldn't parse node." );

				if ( result )
				{
					obj.addObject( node, nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = std::make_shared< SkeletonAnimationBone >( obj );
				result = createBinaryParser< SkeletonAnimationBone >().parse( *bone, chunk );
				checkError( result, "Couldn't parse bone." );

				if ( result )
				{
					obj.addObject( bone, nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationKeyFrame:
				keyFrame = std::make_unique< SkeletonAnimationKeyFrame >( obj, 0_ms );
				result = createBinaryParser< SkeletonAnimationKeyFrame >().parse( *keyFrame, chunk );
				checkError( result, "Couldn't parse keyframe." );

				if ( result )
				{
					obj.addKeyFrame( std::move( keyFrame ) );
				}

				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
