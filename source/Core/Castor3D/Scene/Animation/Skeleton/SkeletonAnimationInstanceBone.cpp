#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceBone.hpp"

#include "Castor3D/Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Castor3D/Mesh/Skeleton/Bone.hpp"

using namespace castor;

namespace castor3d
{
	SkeletonAnimationInstanceBone::SkeletonAnimationInstanceBone( SkeletonAnimationInstance & animationInstance
		, SkeletonAnimationBone & animationObject
		, SkeletonAnimationInstanceObjectPtrArray & allObjects )
		: SkeletonAnimationInstanceObject{ animationInstance, animationObject, allObjects }
		, m_animationBone{ animationObject }
	{
	}

	SkeletonAnimationInstanceBone :: ~SkeletonAnimationInstanceBone()
	{
	}

	void SkeletonAnimationInstanceBone::doApply()
	{
		BoneSPtr bone = m_animationBone.getBone();

		if ( bone )
		{
			m_finalTransform = m_cumulativeTransform * bone->getOffsetMatrix();
		}
	}
}
