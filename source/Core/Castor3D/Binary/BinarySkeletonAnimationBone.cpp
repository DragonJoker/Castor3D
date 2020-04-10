#include "Castor3D/Binary/BinarySkeletonAnimationBone.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationObject.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

using namespace castor;

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

	castor::String BinaryParser< SkeletonAnimationBone >::Name = cuT( "SkeletonAnimationBone" );

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
				checkError( result, "Couldn't parse name." );

				if ( result )
				{
					auto bone = static_cast< Skeleton * >( obj.getOwner()->getOwner() )->findBone( name );

					if ( bone )
					{
						obj.setBone( bone );
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
