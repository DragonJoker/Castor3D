#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	SkeletonAnimationInstanceNode::SkeletonAnimationInstanceNode( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationNode & p_animationObject, SkeletonAnimationInstanceObjectPtrStrMap & p_allObjects )
		: SkeletonAnimationInstanceObject{ p_animationInstance, p_animationObject, p_allObjects }
	{
	}

	SkeletonAnimationInstanceNode :: ~SkeletonAnimationInstanceNode()
	{
	}

	void SkeletonAnimationInstanceNode::doApply()
	{
		m_finalTransform = m_animationObject.getNodeTransform();
	}
}
