#include "MovingObject.hpp"

#include "Animation.hpp"
#include "MovableObject.hpp"
#include "SceneNode.hpp"

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
		m_finalTransform = m_cumulativeTransform;
	}

	MovingObjectBaseSPtr MovingObject::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< MovingObject >();
		l_return->m_object = m_object;
		p_animation.AddMovingObject( l_return, l_return );
		return l_return;
	}
}
