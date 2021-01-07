#include "Aria/Model/TreeModelNode.hpp"

#include "Aria/DatabaseTest.hpp"
#include "Aria/TestDatabase.hpp"

namespace aria
{
	TreeModelNode::TreeModelNode( Renderer renderer
		, TestsCounts & counts )
		: renderer{ renderer }
		, statusName{ NodeType::eRenderer
			, &counts
			, renderer->name }
		, counts{ &counts }
		, m_container{ true }
	{
	}

	TreeModelNode::TreeModelNode( TreeModelNode * parent
		, Renderer renderer
		, Category category
		, TestsCounts & counts )
		: renderer{ renderer }
		, category{ category }
		, statusName{ NodeType::eCategory
			, &counts
			, category->name }
		, counts{ &counts }
		, m_container{ true }
		, m_parent{ parent }
	{
	}

	TreeModelNode::TreeModelNode( TreeModelNode * parent
		, DatabaseTest & test )
		: test{ &test }
		, renderer{ test.getRenderer() }
		, category{ test.getCategory() }
		, statusName{ NodeType::eTestRun
			, nullptr
			, test.getName() }
		, counts{ &test.getCounts() }
		, m_container{ false }
		, m_parent{ parent }
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
