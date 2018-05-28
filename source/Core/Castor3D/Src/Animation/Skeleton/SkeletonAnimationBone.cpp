#include "SkeletonAnimationBone.hpp"

#include "SkeletonAnimation.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

namespace castor3d
{
	SkeletonAnimationBone::SkeletonAnimationBone( SkeletonAnimation & animation )
		: SkeletonAnimationObject{ animation, SkeletonAnimationObjectType::eBone }
	{
	}

	SkeletonAnimationBone :: ~SkeletonAnimationBone()
	{
	}

	String const & SkeletonAnimationBone::getName()const
	{
		return getBone()->getName();
	}
}
