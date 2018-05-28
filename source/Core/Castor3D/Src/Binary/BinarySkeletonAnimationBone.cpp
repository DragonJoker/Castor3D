#include "BinarySkeletonAnimationBone.hpp"

#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Binary/BinarySkeletonAnimationObject.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationBone >::doWrite( SkeletonAnimationBone const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = BinaryWriter< SkeletonAnimationObject >{}.write( obj, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationBone >::doParse( SkeletonAnimationBone & obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					auto bone = static_cast< Skeleton * >( obj.getOwner()->getOwner() )->findBone( name );

					if ( bone )
					{
						obj.setBone( bone );
					}
					else
					{
						Logger::logError( cuT( "Couldn't find bone " ) + name + cuT( " in skeleton" ) );
					}
				}

				break;

			case ChunkType::eAnimationObject:
				result = createBinaryParser< SkeletonAnimationObject >().parse( obj, chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
