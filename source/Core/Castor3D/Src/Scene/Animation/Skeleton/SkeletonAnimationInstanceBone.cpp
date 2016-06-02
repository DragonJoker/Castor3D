#include "SkeletonAnimationInstanceBone.hpp"

#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Mesh/Skeleton/Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	SkeletonAnimationInstanceBone::SkeletonAnimationInstanceBone( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationBone & p_animationObject )
		: SkeletonAnimationInstanceObject{ p_animationInstance, p_animationObject }
		, m_animationBone{ p_animationObject }
	{
	}

	SkeletonAnimationInstanceBone :: ~SkeletonAnimationInstanceBone()
	{
	}

	void SkeletonAnimationInstanceBone::DoApply()
	{
		BoneSPtr l_bone = m_animationBone.GetBone();

		if ( l_bone )
		{
			m_finalTransform = m_cumulativeTransform * l_bone->GetOffsetMatrix();
		}
	}
}
