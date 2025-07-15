#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"

#include "Castor3D/Model/Skeleton/BoneNode.hpp"

CU_ImplementSmartPtr( c3d, SkeletonAnimationBone )

namespace c3d
{
	SkeletonAnimationBone::SkeletonAnimationBone( SkeletonAnimation & animation )
		: SkeletonAnimationObject{ animation, SkeletonNodeType::eBone }
	{
	}

	String const & SkeletonAnimationBone::getName()const
	{
		return getBone()->getName();
	}

	NodeTransform const & SkeletonAnimationBone::getNodeTransform()const noexcept
	{
		return getBone()->getTransform();
	}
}
