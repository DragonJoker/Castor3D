#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"

#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"

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
}
