#include "Castor3D/Animation/Skeleton/SkeletonAnimationBone.hpp"

#include "Castor3D/Mesh/Skeleton/Bone.hpp"

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
