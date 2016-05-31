#include "SkeletonAnimationNode.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationNode >::DoWrite( SkeletonAnimationNode const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, m_chunk );
		}

		if ( l_return )
		{
			l_return = BinaryWriter< AnimationObject >{}.Write( p_obj, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationNode >::DoParse( SkeletonAnimationNode & p_obj )
	{
		bool l_return = true;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_NAME:
				l_return = DoParseChunk( p_obj.m_name, l_chunk );
				break;

			case eCHUNK_TYPE_ANIMATION_OBJECT:
				l_return = BinaryParser< AnimationObject >{}.Parse( p_obj, l_chunk );
				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	SkeletonAnimationNode::SkeletonAnimationNode( Animation & p_animation, String const & p_name )
		: AnimationObject{ p_animation, eANIMATION_OBJECT_TYPE_NODE }
		, m_name{ p_name }
	{
	}

	SkeletonAnimationNode :: ~SkeletonAnimationNode()
	{
	}

	void SkeletonAnimationNode::DoApply()
	{
		m_finalTransform = m_nodeTransform;
	}

	AnimationObjectSPtr SkeletonAnimationNode::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< SkeletonAnimationNode >( p_animation, m_name );
		p_animation.AddObject( l_return, l_return );
		return l_return;
	}

	//*************************************************************************************************
}
