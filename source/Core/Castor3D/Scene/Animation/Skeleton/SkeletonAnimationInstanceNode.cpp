#include "SkeletonAnimationInstanceNode.hpp"

#include "Animation/Skeleton/SkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	SkeletonAnimationInstanceNode::SkeletonAnimationInstanceNode( SkeletonAnimationInstance & animationInstance
		, SkeletonAnimationNode & animationObject
		, SkeletonAnimationInstanceObjectPtrArray & allObjects )
		: SkeletonAnimationInstanceObject{ animationInstance, animationObject, allObjects }
	{
	}

	SkeletonAnimationInstanceNode :: ~SkeletonAnimationInstanceNode()
	{
	}

	void SkeletonAnimationInstanceNode::doApply()
	{
		m_finalTransform = m_cumulativeTransform;
	}
}
