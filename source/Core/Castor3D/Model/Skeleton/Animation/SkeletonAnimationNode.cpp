#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"

#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"

CU_ImplementCUSmartPtr( castor3d, SkeletonAnimationNode )

namespace castor3d
{
	SkeletonAnimationNode::SkeletonAnimationNode( SkeletonAnimation & animation )
		: SkeletonAnimationObject{ animation, SkeletonNodeType::eNode }
	{
	}

	castor::String const & SkeletonAnimationNode::getName()const
	{
		return getNode()->getName();
	}

	NodeTransform const & SkeletonAnimationNode::getNodeTransform()const noexcept
	{
		return getNode()->getTransform();
	}
}
