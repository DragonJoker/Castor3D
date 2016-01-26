#include "MovingNode.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingNode::MovingNode()
		: MovingObjectBase( eMOVING_OBJECT_TYPE_NODE )
	{
	}

	MovingNode :: ~MovingNode()
	{
	}

	String const & MovingNode::GetName()const
	{
		return cuEmptyString;
	}

	void MovingNode::DoApply()
	{
		m_finalTransform = m_nodeTransform;
	}

	MovingObjectBaseSPtr MovingNode::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< MovingNode >();
		p_animation.AddMovingObject( l_return, l_return );
		return l_return;
	}
}
