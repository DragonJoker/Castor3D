#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"

#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	SkeletonNode::SkeletonNode( Type type
		, castor::String name
		, Skeleton & skeleton )
		: castor::Named{ std::move( name ) }
		, m_type{ type }
		, m_skeleton{ skeleton }
	{
	}

	SkeletonNode::SkeletonNode( castor::String name
		, Skeleton & skeleton )
		: SkeletonNode{ eNode, std::move( name ), skeleton }
	{
	}

	void SkeletonNode::addChild( SkeletonNode & node )
	{
		if ( m_children.end() == m_children.find( node.getName() ) )
		{
			m_children.emplace( node.getName(), &node );
		}
	}

	void SkeletonNode::setParent( SkeletonNode & node )
	{
		m_parent = &node;
	}
}
