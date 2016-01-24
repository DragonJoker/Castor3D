#include "MovingObject.hpp"
#include "MovableObject.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingObject::MovingObject()
		: MovingObjectBase( eMOVING_OBJECT_TYPE_OBJECT )
	{
	}

	MovingObject::~MovingObject()
	{
	}

	String const & MovingObject::GetName()const
	{
		return GetObject()->GetName();
	}

	void MovingObject::DoApply()
	{
		m_finalTransformation = m_transformations;
	}

	MovingObjectBaseSPtr MovingObject::DoClone()
	{
		return std::make_shared< MovingObject >( *this );
	}
}
