#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"

#include "Castor3D/Model/Skeleton/Skeleton.hpp"

CU_ImplementSmartPtr( c3d, SkeletonNode )

namespace c3d
{
	SkeletonNode::SkeletonNode( SkeletonNodeType type
		, String name
		, Skeleton & skeleton )
		: Named{ c3d::move( name ) }
		, m_type{ type }
		, m_skeleton{ skeleton }
	{
	}

	SkeletonNode::SkeletonNode( String name
		, Skeleton & skeleton )
		: SkeletonNode{ SkeletonNodeType::eNode, c3d::move( name ), skeleton }
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

	SkeletonNodeUPtr SkeletonNode::clone( Skeleton & parent )const
	{
		auto result = makeUnique< SkeletonNode >( getName(), parent );
		doCloneInto( *result );
		return result;
	}

	void SkeletonNode::doCloneInto( SkeletonNode & output )const
	{
		output.m_transform = m_transform;
	}
}
