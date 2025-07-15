#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"

#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"

CU_ImplementSmartPtr( c3d, SkeletonAnimationNode )

namespace c3d
{
	SkeletonAnimationNode::SkeletonAnimationNode( SkeletonAnimation & animation )
		: SkeletonAnimationObject{ animation, SkeletonNodeType::eNode }
	{
	}

	String const & SkeletonAnimationNode::getName()const
	{
		return getNode()->getName();
	}

	NodeTransform const & SkeletonAnimationNode::getNodeTransform()const noexcept
	{
		return getNode()->getTransform();
	}
}
