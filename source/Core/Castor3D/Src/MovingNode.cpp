#include "MovingNode.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	uint32_t MovingNode::s_count = 0;

	MovingNode::MovingNode()
		: MovingObjectBase( eMOVING_OBJECT_TYPE_NODE )
		, m_name( string::to_string( s_count++ ) )
	{
	}

	MovingNode :: ~MovingNode()
	{
	}

	String const & MovingNode::GetName()const
	{
		return m_name;
	}

	void MovingNode::DoApply()
	{
		m_finalTransform = m_nodeTransform;
	}

	MovingObjectBaseSPtr MovingNode::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< MovingNode >();
		l_return->m_name = m_name;
		p_animation.AddMovingObject( l_return, l_return );
		return l_return;
	}
}
