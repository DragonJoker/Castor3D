#include "SkeletonAnimationNode.hpp"

#include "SkeletonAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationNode >::DoWrite( SkeletonAnimationNode const & p_obj )
	{
		bool l_result = true;

		if ( l_result )
		{
			l_result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( l_result )
		{
			l_result = BinaryWriter< SkeletonAnimationObject >{}.Write( p_obj, m_chunk );
		}

		return l_result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationNode >::DoParse( SkeletonAnimationNode & p_obj )
	{
		bool l_result = true;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_result && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eName:
				l_result = DoParseChunk( p_obj.m_name, l_chunk );
				break;

			case ChunkType::eAnimationObject:
				l_result = BinaryParser< SkeletonAnimationObject >{}.Parse( p_obj, l_chunk );
				break;
			}
		}

		return l_result;
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
