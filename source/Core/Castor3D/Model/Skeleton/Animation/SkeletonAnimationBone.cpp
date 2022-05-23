#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"

#include "Castor3D/Model/Skeleton/BoneNode.hpp"

namespace castor3d
{
	SkeletonAnimationBone::SkeletonAnimationBone( SkeletonAnimation & animation )
		: SkeletonAnimationObject{ animation, SkeletonNodeType::eBone }
	{
	}

	castor::String const & SkeletonAnimationBone::getName()const
	{
		return getBone()->getName();
	}

	NodeTransform const & SkeletonAnimationBone::getNodeTransform()const noexcept
	{
		return getBone()->getTransform();
	}
}
