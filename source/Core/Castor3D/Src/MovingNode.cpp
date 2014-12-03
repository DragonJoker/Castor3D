#include "MovingNode.hpp"

using namespace Castor;

namespace Castor3D
{
	const String MovingNode::StrVoidString;

	MovingNode::MovingNode()
		:	MovingObjectBase( eMOVING_OBJECT_TYPE_NODE )
	{
	}

	MovingNode::~MovingNode()
	{
	}

	String const & MovingNode::GetName()const
	{
		return StrVoidString;
	}

	void MovingNode::DoApply()
	{
		m_mtxFinalTransformation = m_mtxTransformations;
	}

	MovingObjectBaseSPtr MovingNode::DoClone()
	{
		return std::make_shared< MovingNode >( *this );
	}
}
