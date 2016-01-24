#include "AnimatedObject.hpp"

#include "MovableObject.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedObject::AnimatedObject()
		: Named( cuT( "" ) )
		, m_animations( NULL )
	{
	}

	AnimatedObject::AnimatedObject( MovableObjectSPtr p_object, AnimationPtrStrMap * p_animations )
		: Named( p_object->GetName() )
		, m_animations( p_animations )
		, m_object( p_object )
	{
	}

	AnimatedObject::AnimatedObject( AnimationPtrStrMap * p_animations )
		: Named( cuT( "" ) )
		, m_animations( p_animations )
	{
	}

	AnimatedObject :: ~AnimatedObject()
	{
	}

	void AnimatedObject::SetObject( MovableObjectSPtr p_object )
	{
		m_object = p_object;

		if ( p_object )
		{
			m_name = p_object->GetName();
		}
		else
		{
			m_name.clear();
		}
	}
}
