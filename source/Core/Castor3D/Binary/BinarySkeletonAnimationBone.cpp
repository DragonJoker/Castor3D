#include "Castor3D/Binary/BinarySkeletonAnimationBone.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationObject.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationBone >::doWrite( SkeletonAnimationBone const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		if ( result )
		{
			result = BinaryWriter< SkeletonAnimationObject >{}.write( obj, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< SkeletonAnimationBone >::Name = cuT( "SkeletonAnimationBone" );

	bool BinaryParser< SkeletonAnimationBone >::doParse( SkeletonAnimationBone & obj )
	{
		bool result = true;
		castor::String name;
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
					auto bone = static_cast< Skeleton * >( obj.getOwner()->getAnimable() )->findNode( name );

					if ( bone )
					{
						if ( bone->getType() == SkeletonNode::eBone )
						{
							obj.setBone( static_cast< BoneNode & >( *bone ) );
						}
						else
						{
							log::error << cuT( "Skeleton node with name " ) << name << cuT( " is not a bone" ) << std::endl;
						}
					}
					else
					{
						log::error << cuT( "Couldn't find bone " ) << name << cuT( " in skeleton" ) << std::endl;
					}
				}

				break;

			case ChunkType::eAnimationObject:
				result = createBinaryParser< SkeletonAnimationObject >().parse( obj, chunk );
				checkError( result, "Couldn't parse object." );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
