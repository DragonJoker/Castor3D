#include "MovingNode.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingNode::MovingNode()
		: MovingObjectBase( eMOVING_OBJECT_TYPE_NODE )
	{
	}

	MovingNode::~MovingNode()
	{
	}

	String const & MovingNode::GetName()const
	{
		return cuEmptyString;
	}

	void MovingNode::DoApply()
	{
		m_finalTransformation = m_transformations;
	}

	MovingObjectBaseSPtr MovingNode::DoClone()
	{
		return std::make_shared< MovingNode >( *this );
	}
}
