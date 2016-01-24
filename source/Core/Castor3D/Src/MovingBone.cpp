#include "MovingBone.hpp"

#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingBone::MovingBone()
		: MovingObjectBase()
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
			l_bone->SetFinalTransformation( m_transformations * l_bone->GetOffsetMatrix() );
		}
	}
}
