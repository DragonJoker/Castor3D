#include "MovingNode.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingNode::MovingNode()
		: MovingObjectBase()
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
	}
}
