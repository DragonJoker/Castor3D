#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"

#include "Castor3D/Model/Skeleton/Skeleton.hpp"

CU_ImplementSmartPtr( castor3d, SkeletonNode )

namespace castor3d
{
	SkeletonNode::SkeletonNode( SkeletonNodeType type
		, castor::String name
		, Skeleton & skeleton )
		: castor::Named{ castor::move( name ) }
		, m_type{ type }
		, m_skeleton{ skeleton }
	{
	}

	SkeletonNode::SkeletonNode( castor::String name
		, Skeleton & skeleton )
		: SkeletonNode{ SkeletonNodeType::eNode, castor::move( name ), skeleton }
	{
	}

	void SkeletonNode::addChild( SkeletonNode & node )
	{
		if ( m_children.end() == m_children.find( node.getName() ) )
		{
			m_children.try_emplace( node.getName(), &node );
		}
	}

	void SkeletonNode::setParent( SkeletonNode & node )
	{
		m_parent = &node;
	}
}
