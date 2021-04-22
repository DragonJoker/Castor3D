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
		TreeModelNode( Renderer renderer
			, RendererTestsCounts & counts );
		TreeModelNode( TreeModelNode * parent
			, Renderer renderer
			, Category category
			, CategoryTestsCounts & counts );
		TreeModelNode( TreeModelNode * parent
			, DatabaseTest & test );
		~TreeModelNode();

		bool isRootNode()const
		{
			return GetParent() == nullptr;
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

		void Insert( TreeModelNode * child, size_t n )
		{
			m_children.insert( m_children.begin() + n, child );
		}

		void Append( TreeModelNode * child )
		{
			m_children.push_back( child );
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
		DatabaseTest * test{};
		Renderer renderer{};
		Category category{};
		StatusName statusName{};
		AllTestsCounts const * allCounts{};
		RendererTestsCounts const * rendererCounts{};
		CategoryTestsCounts const * categoryCounts{};

	private:
		bool m_container{};
		TreeModelNode * m_parent{};
		TreeModelNodePtrArray m_children;
	};
}

#endif
