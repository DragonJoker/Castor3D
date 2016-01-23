#include "MovingBone.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingBone::MovingBone()
		: MovingObjectBase( eMOVING_OBJECT_TYPE_BONE )
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
		m_finalTransformation = m_transformations * GetBone()->GetOffsetMatrix();
	}

	MovingObjectBaseSPtr MovingBone::DoClone()
	{
		return std::make_shared< MovingBone >( *this );
	}
}
