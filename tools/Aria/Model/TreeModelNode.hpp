/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TreeModelNode_HPP___
#define ___CTP_TreeModelNode_HPP___

#include "Aria/Prerequisites.hpp"

namespace aria
{
	class TreeModelNode
	{
	public:
		TreeModelNode( TreeModelNode * parent
			, wxString const & name
			, bool isRenderer = false );
		TreeModelNode( TreeModelNode * parent
			, Test & test );
		~TreeModelNode();

		bool isRenderer()const
		{
			return m_isRenderer;
		}

		bool isRootNode()const
		{
			return GetParent() == nullptr;
		}

		TreeModelNode * getRenderer()const
		{
			assert( !m_isRenderer && !category.empty() );
			return m_parent;
		}
	
		void Remove( TreeModelNode * node );

		TreeModelNode * GetParent()const
		{
			return m_parent;
		}

		TreeModelNodePtrArray & GetChildren()
		{
			return m_children;
		}

		TreeModelNode * GetNthChild( size_t n )const
		{
			return m_children[n];
		}

		void Insert( TreeModelNode * p_child, size_t n )
		{
			m_children.insert( m_children.begin() + n, p_child );
		}

		void Append( TreeModelNode * p_child )
		{
			m_children.push_back( p_child );
		}

		size_t GetChildCount()const
		{
			return m_children.size();
		}

		bool IsContainer()const
		{
			return m_container;
		}

	public:
		wxString category;
		Test * test{};

	private:
		bool m_container{};
		bool m_isRenderer{};
		TreeModelNode * m_parent;
		TreeModelNodePtrArray m_children;
	};
}

#endif
