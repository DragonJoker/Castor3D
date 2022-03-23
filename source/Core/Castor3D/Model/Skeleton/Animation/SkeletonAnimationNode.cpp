#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"

namespace castor3d
{
	SkeletonAnimationNode::SkeletonAnimationNode( SkeletonAnimation & animation
		, castor::String const & name )
		: SkeletonAnimationObject{ animation, SkeletonAnimationObjectType::eNode }
		, m_name{ name }
	{
	}
}
