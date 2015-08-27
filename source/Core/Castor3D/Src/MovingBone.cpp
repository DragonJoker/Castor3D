#include "MovingBone.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingBone::MovingBone()
		:	MovingObjectBase( eMOVING_OBJECT_TYPE_BONE )
	{
	}

	MovingBone::~MovingBone()
	{
	}

	String const & MovingBone::GetName()const
	{
		return GetBone()->GetName();
	}

	void MovingBone::DoApply()
	{
		m_mtxFinalTransformation = m_mtxTransformations * GetBone()->GetOffsetMatrix();
	}

	MovingObjectBaseSPtr MovingBone::DoClone()
	{
		return std::make_shared< MovingBone >( *this );
	}
}
