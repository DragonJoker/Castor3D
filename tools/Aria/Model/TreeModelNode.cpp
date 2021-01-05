#include "Aria/Model/TreeModelNode.hpp"

#include "Aria/TestDatabase.hpp"

namespace aria
{
	TreeModelNode::TreeModelNode( TreeModelNode * parent
		, Renderer renderer
		, Category category )
		: renderer{ renderer }
		, category{ category }
		, m_parent{ parent }
		, m_container{ true }
	{
	}

	TreeModelNode::TreeModelNode( TreeModelNode * parent
		, DatabaseTest & test )
		: test{ &test }
		, renderer{ test->renderer }
		, m_parent{ parent }
		, m_container{ false }
	{
	}

	TreeModelNode::~TreeModelNode()
	{
		// free all our children nodes
		size_t count = m_children.size();

		for( size_t i = 0; i < count; i++ )
		{
			delete m_children[i];
		}

		m_children.clear();
	}

	void TreeModelNode::Remove( TreeModelNode * node )
	{
		auto it = std::find( m_children.begin()
			, m_children.end()
			, node );

		if( it != m_children.end() )
		{
			m_children.erase( it );
		}
	}
}
