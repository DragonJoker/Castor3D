#include "MovingBone.hpp"

#include "Animation.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingBone::MovingBone()
		: MovingObjectBase( eMOVING_OBJECT_TYPE_BONE )
	{
	}

	MovingBone :: ~MovingBone()
	{
	}

	String const & MovingBone::GetName()const
	{
		return GetBone()->GetName();
	}

	void MovingBone::DoApply()
	{
		BoneSPtr l_bone = GetBone();

		if ( l_bone )
		{
			m_finalTransform = m_cumulativeTransform * l_bone->GetOffsetMatrix();
		}
	}

	MovingObjectBaseSPtr MovingBone::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< MovingBone >();
		l_return->m_bone = m_bone;
		p_animation.AddMovingObject( l_return, l_return );
		return l_return;
	}
}
