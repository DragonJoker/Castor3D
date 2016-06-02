#include "SkeletonAnimatedNode.hpp"

#include "SkeletonAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	SkeletonAnimatedNode::SkeletonAnimatedNode( SkeletonAnimation & p_animation, String const & p_name )
		: SkeletonAnimationObject{ p_animation, AnimationObjectType::Node }
		, m_name{ p_name }
	{
	}

	SkeletonAnimatedNode :: ~SkeletonAnimatedNode()
	{
	}

	void SkeletonAnimationNode::DoApply()
	{
		m_finalTransform = m_nodeTransform;
	}
}
