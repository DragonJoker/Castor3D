#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"

CU_ImplementCUSmartPtr( castor3d, SkeletonAnimationInstanceBone )

namespace castor3d
{
	SkeletonAnimationInstanceBone::SkeletonAnimationInstanceBone( SkeletonAnimationInstance & animationInstance
		, SkeletonAnimationBone & animationObject
		, SkeletonAnimationInstanceObjectPtrArray & allObjects )
		: SkeletonAnimationInstanceObject{ animationInstance, animationObject, allObjects }
		, m_animationBone{ animationObject }
	{
	}

	void SkeletonAnimationInstanceBone::doApply()
	{
		auto bone = m_animationBone.getBone();

		if ( bone )
		{
			m_finalTransform = m_cumulativeTransform * bone->getInverseTransform();
		}
	}
}
