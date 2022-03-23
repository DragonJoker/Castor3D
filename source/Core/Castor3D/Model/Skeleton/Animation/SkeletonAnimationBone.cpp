#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"

#include "Castor3D/Model/Skeleton/Bone.hpp"

namespace castor3d
{
	SkeletonAnimationBone::SkeletonAnimationBone( SkeletonAnimation & animation )
		: SkeletonAnimationObject{ animation, SkeletonAnimationObjectType::eBone }
	{
	}

	castor::String const & SkeletonAnimationBone::getName()const
	{
		return getBone()->getName();
	}
}
