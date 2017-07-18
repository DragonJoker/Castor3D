#include "SkeletonAnimationNode.hpp"

#include "SkeletonAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationNode >::DoWrite( SkeletonAnimationNode const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = BinaryWriter< SkeletonAnimationObject >{}.Write( p_obj, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationNode >::DoParse( SkeletonAnimationNode & p_obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk;

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eName:
				result = DoParseChunk( p_obj.m_name, chunk );
				break;

			case ChunkType::eAnimationObject:
				result = BinaryParser< SkeletonAnimationObject >{}.Parse( p_obj, chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	SkeletonAnimationNode::SkeletonAnimationNode( SkeletonAnimation & p_animation, String const & p_name )
		: SkeletonAnimationObject{ p_animation, SkeletonAnimationObjectType::eNode }
		, m_name{ p_name }
	{
	}

	SkeletonAnimationNode :: ~SkeletonAnimationNode()
	{
	}

	//*************************************************************************************************
}
