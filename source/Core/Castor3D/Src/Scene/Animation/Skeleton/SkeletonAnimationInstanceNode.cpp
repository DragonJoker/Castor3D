#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimationNode.hpp"

using namespace Castor;

namespace Castor3D
{
	SkeletonAnimationInstanceNode::SkeletonAnimationInstanceNode( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationNode & p_animationObject )
		: SkeletonAnimationInstanceObject{ p_animationInstance, p_animationObject }
	{
	}

	SkeletonAnimationInstanceNode :: ~SkeletonAnimationInstanceNode()
	{
	}

	void SkeletonAnimationInstanceNode::DoApply()
	{
		m_finalTransform = m_animationObject.GetNodeTransform();
	}
}
