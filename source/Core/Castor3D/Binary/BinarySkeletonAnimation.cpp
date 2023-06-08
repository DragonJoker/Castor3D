#include "Castor3D/Binary/BinarySkeletonAnimation.hpp"

#include "Castor3D/Binary/BinarySkeletonAnimationBone.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationNode.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimation >::doWrite( SkeletonAnimation const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		for ( auto moving : obj.getRootObjects() )
		{
			switch ( moving->getType() )
			{
			case SkeletonNodeType::eNode:
				result = result && BinaryWriter< SkeletonAnimationNode >{}.write( static_cast< SkeletonAnimationNode const & >( *moving ), m_chunk );
				break;

			case SkeletonNodeType::eBone:
				result = result && BinaryWriter< SkeletonAnimationBone >{}.write( static_cast< SkeletonAnimationBone const & >( *moving ), m_chunk );
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
		SkeletonAnimationNodeUPtr node{};
		SkeletonAnimationBoneUPtr bone{};
		SkeletonAnimationKeyFrameUPtr keyFrame;
		castor::String name;
		BinaryChunk chunk{ doIsLittleEndian() };

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
				node = castor::makeUnique< SkeletonAnimationNode >( obj );
				result = createBinaryParser< SkeletonAnimationNode >().parse( *node, chunk );
				checkError( result, "Couldn't parse node." );

				if ( result )
				{
					obj.addObject( castor::ptrRefCast< SkeletonAnimationObject >( node ), nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = castor::makeUnique< SkeletonAnimationBone >( obj );
				result = createBinaryParser< SkeletonAnimationBone >().parse( *bone, chunk );
				checkError( result, "Couldn't parse bone." );

				if ( result )
				{
					obj.addObject( castor::ptrRefCast< SkeletonAnimationObject >( bone ), nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationKeyFrame:
				keyFrame = castor::makeUnique< SkeletonAnimationKeyFrame >( obj, 0_ms );
				result = createBinaryParser< SkeletonAnimationKeyFrame >().parse( *keyFrame, chunk );
				checkError( result, "Couldn't parse keyframe." );

				if ( result )
				{
					obj.addKeyFrame( castor::ptrRefCast< AnimationKeyFrame >( keyFrame ) );
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
