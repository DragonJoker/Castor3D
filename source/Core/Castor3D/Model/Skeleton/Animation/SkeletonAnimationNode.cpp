#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	SkeletonAnimationNode::SkeletonAnimationNode( SkeletonAnimation & animation
		, String const & name )
		: SkeletonAnimationObject{ animation, SkeletonAnimationObjectType::eNode }
		, m_name{ name }
	{
	}
}
