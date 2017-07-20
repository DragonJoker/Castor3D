#include "SkeletonAnimationInstanceBone.hpp"

#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Mesh/Skeleton/Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	SkeletonAnimationInstanceBone::SkeletonAnimationInstanceBone( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationBone & p_animationObject, SkeletonAnimationInstanceObjectPtrStrMap & p_allObjects )
		: SkeletonAnimationInstanceObject{ p_animationInstance, p_animationObject, p_allObjects }
		, m_animationBone{ p_animationObject }
	{
	}

	SkeletonAnimationInstanceBone :: ~SkeletonAnimationInstanceBone()
	{
	}

	void SkeletonAnimationInstanceBone::DoApply()
	{
		BoneSPtr bone = m_animationBone.GetBone();

		if ( bone )
		{
			m_finalTransform = m_cumulativeTransform * bone->GetOffsetMatrix();
		}
	}
}
