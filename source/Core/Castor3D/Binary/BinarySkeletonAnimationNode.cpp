#include "Castor3D/Binary/BinarySkeletonAnimationNode.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationObject.hpp"
#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationNode >::doWrite( SkeletonAnimationNode const & obj )
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
	castor::String BinaryParserBase< SkeletonAnimationNode >::Name = cuT( "SkeletonAnimationNode" );

	bool BinaryParser< SkeletonAnimationNode >::doParse( SkeletonAnimationNode & obj )
	{
		bool result = true;
		castor::String name;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				if ( m_fileVersion > Version{ 1, 5, 0 } )
				{
					result = doParseChunk( name, chunk );
					checkError( result, "Couldn't parse name." );

					if ( result )
					{
						auto node = static_cast< Skeleton * >( obj.getOwner()->getAnimable() )->findNode( name );

						if ( node )
						{
							if ( node->getType() == SkeletonNodeType::eNode )
							{
								obj.setNode( *node );
							}
							else
							{
								result = false;
								checkError( result, cuT( "Skeleton node with name " ) + name + cuT( " is not a node" ) );
							}
						}
						else
						{
							result = false;
							checkError( result, cuT( "Couldn't find node " ) + name + cuT( " in skeleton" ) );
						}
					}
				}
				break;
			case ChunkType::eAnimationObject:
				result = createBinaryParser< SkeletonAnimationObject >().parse( obj, chunk );
				checkError( result, "Couldn't parse animation object." );
				break;
			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationNode >::doParse_v1_5( SkeletonAnimationNode & obj )
	{
		bool result = true;
		castor::String name;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			if ( m_fileVersion <= Version{ 1, 5, 0 }
				&& chunk.getChunkType() == ChunkType::eName )
			{
				result = doParseChunk( name, chunk );
				checkError( result, "Couldn't parse name." );

				if ( result )
				{
					auto node = static_cast< Skeleton * >( obj.getOwner()->getAnimable() )->createNode( name );
					obj.setNode( *node );
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
