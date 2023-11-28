/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelinesNodes_H___
#define ___C3D_PipelinesNodes_H___

#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#define C3D_EnsureNodesCounts 1

namespace castor3d
{
	template< typename NodeT, uint64_t CountT >
	struct NodesViewT
	{
		using CountedNode = CountedNodeT< NodeT >;

		static uint64_t constexpr maxNodes = CountT;
		static uint64_t constexpr maxCount = CountT;

		explicit NodesViewT( CountedNode * data )
			: m_nodes{ data }
			, m_count{}
		{
		}

		NodesViewT()
			: NodesViewT{ nullptr }
		{
		}

		auto emplace( CountedNode & node )
		{
			CU_Assert( size() < maxNodes
				, "Too many nodes for given buffer and given pipeline" );
#if C3D_EnsureNodesCounts
			if ( size() == maxNodes )
			{
				CU_Exception( "Too many nodes for given buffer and given pipeline" );
			}
#endif

			auto it = end();
			*it = &node;
			++m_count;
		}

		void clear()noexcept
		{
			m_count = 0u;
		}

		auto begin()noexcept
		{
			return m_nodes.data();
		}

		auto begin()const noexcept
		{
			return m_nodes.data();
		}

		auto end()noexcept
		{
			return std::next( begin(), ptrdiff_t( m_count ) );
		}

		auto end()const noexcept
		{
			return std::next( begin(), ptrdiff_t( m_count ) );
		}

		auto & front()noexcept
		{
			return *begin();
		}

		auto & front()const noexcept
		{
			return *begin();
		}

		auto size()const noexcept
		{
			return m_count;
		}

		auto empty()const noexcept
		{
			return size() == 0;
		}

	private:
		std::array< CountedNode *, maxNodes > m_nodes;
		size_t m_count;
	};

	template< typename NodeT >
	struct BuffersNodesViewT
	{
		using CountedNode = CountedNodeT< NodeT >;
		using NodesView = NodesViewT< NodeT, 4096u >;

		static uint64_t constexpr maxBuffers = 16ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxBuffers;

		struct BufferNodes
		{
			ashes::BufferBase const * buffer{};
			NodesView nodes;
		};

		explicit BuffersNodesViewT( CountedNode * data )
			: m_buffers{ maxBuffers }
			, m_count{}
		{
			if ( data )
			{
				for ( auto & buffer : m_buffers )
				{
					buffer.nodes = NodesView{ data };
					data += NodesView::maxCount;
				}
			}
		}

		BuffersNodesViewT()
			: BuffersNodesViewT{ nullptr }
		{
		}

		auto emplace( ashes::BufferBase const & buffer )
		{
			auto it = std::find_if( begin()
				, end()
				, [&buffer]( BufferNodes const & lookup )
				{
					return lookup.buffer == &buffer;
				} );

			if ( it == end() )
			{
				CU_Assert( m_count < maxBuffers
					, "Too many buffers for given pipeline" );
#if C3D_EnsureNodesCounts
				if ( m_count == maxBuffers )
				{
					CU_Exception( "Too many buffers for given pipeline" );
				}
#endif

				it->buffer = &buffer;
				++m_count;
			}

			return it;
		}

		void emplace( ashes::BufferBase const & buffer
			, CountedNode & node )
		{
			auto it = emplace( buffer );
			it->nodes.emplace( node );
			CU_Require( !it->nodes.empty() );
		}

		void clear()noexcept
		{
			for ( auto & [buffer, nodes] : m_buffers )
			{
				buffer = {};
				nodes.clear();
			}

			m_count = 0u;
		}

		auto begin()noexcept
		{
			return m_buffers.begin();
		}

		auto begin()const noexcept
		{
			return m_buffers.begin();
		}

		auto end()noexcept
		{
			return std::next( begin(), ptrdiff_t( m_count ) );
		}

		auto end()const noexcept
		{
			return std::next( begin(), ptrdiff_t( m_count ) );
		}

		auto size()const noexcept
		{
			return m_count;
		}

		auto empty()const noexcept
		{
			return size() == 0;
		}

	private:
		std::vector< BufferNodes > m_buffers;
		uint32_t m_count;
	};

	template< typename NodeT >
	class PipelinesNodesT
	{
	public:
		using CountedNode = CountedNodeT< NodeT >;
		using NodeArray = NodeArrayT< NodeT >;
		using NodesView = BuffersNodesViewT< NodeT >;

		static uint64_t constexpr maxPipelines = 256ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxPipelines;

		struct PipelineNodes
		{
			PipelineAndID pipeline{};
			bool isFrontCulled{};
			NodesView nodes{};
		};

		PipelinesNodesT()
			: m_nodes{ maxCount }
			, m_nodeCount{}
		{
		}

		auto emplace( PipelineAndID const & pipeline
			, bool isFrontCulled )
		{
			auto id = uint32_t( pipeline.id + ( isFrontCulled ? ( maxPipelines / 2u ) : 0u ) );
			auto it = m_pipelines.find( id );

			if ( it == m_pipelines.end() )
			{
				CU_Assert( pipeline.id < maxPipelines
					, "Too many pipelines" );
#if C3D_EnsureNodesCounts
				if ( pipeline.id >= maxPipelines / 2u )
				{
					CU_Exception( "Too many pipelines" );
				}
#endif

				auto data = m_nodes.data() + ( m_pipelines.size() * NodesView::maxCount );
				it = m_pipelines.emplace( id
					, PipelineNodes{ pipeline, isFrontCulled, NodesView{ data } } ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, ashes::BufferBase const & buffer
			, CountedNode const & node
			, uint32_t drawCount
			, bool isFrontCulled )
		{
			auto ires = m_countedNodes.emplace( node.node, nullptr );

			if ( ires.second )
			{
				CU_Assert( m_nodeCount < maxCount
					, "Too many nodes" );
#if C3D_EnsureNodesCounts
				if ( m_nodeCount == maxCount )
				{
					CU_Exception( "Too many nodes" );
				}
#endif
				m_nodes[m_nodeCount] = node;
				ires.first->second = &m_nodes[m_nodeCount];
				++m_nodeCount;

				auto it = emplace( pipeline, isFrontCulled );
				it->nodes.emplace( buffer, *ires.first->second );
			}
			else
			{
				ires.first->second->visible = true;
			}
		}

		void erase( NodeT const & node )noexcept
		{
			auto it = m_countedNodes.find( &node );

			if ( it != m_countedNodes.end() )
			{
				it->second->visible = false;
			}
		}

		void clear()noexcept
		{
			m_pipelines.clear();
		}

		auto begin()noexcept
		{
			return m_pipelines.begin();
		}

		auto begin()const noexcept
		{
			return m_pipelines.begin();
		}

		auto end()noexcept
		{
			return m_pipelines.end();
		}

		auto end()const noexcept
		{
			return m_pipelines.end();
		}

		auto size()const noexcept
		{
			return m_pipelines.size();
		}

		auto nodesCount()const noexcept
		{
			return m_nodeCount;
		}

		auto empty()const noexcept
		{
			return m_pipelines.empty();
		}

	private:
		NodeArray m_nodes;
		uint32_t m_nodeCount;
		std::map< uint32_t, PipelineNodes > m_pipelines;
		std::unordered_map< NodeT const *, CountedNode * > m_countedNodes;
	};

	template< typename NodeT >
	class PipelinesDrawnNodesT
	{
	public:
		using CountedNode = CountedNodeT< NodeT >;
		using NodesView = BuffersNodesViewT< NodeT >;

		static uint64_t constexpr maxPipelines = 256ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxPipelines;

		struct PipelineNodes
		{
			PipelineAndID pipeline{};
			NodesView nodes;
		};

		PipelinesDrawnNodesT()
		{
		}

		auto emplace( PipelineAndID const & pipeline
			, bool isFrontCulled )
		{
			auto id = uint32_t( pipeline.id + ( isFrontCulled ? ( maxPipelines / 2u ) : 0u ) );
			auto it = m_pipelines.find( id );

			if ( it == m_pipelines.end() )
			{
				it = m_pipelines.emplace( id, PipelineNodes{ pipeline, NodesView{} } ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, ashes::BufferBase const & buffer
			, CountedNode const & node
			, bool isFrontCulled )
		{
			auto it = emplace( pipeline, isFrontCulled );
			it->buffers.emplace( buffer, node );
		}

		void clear()noexcept
		{
			m_pipelines.clear();
		}

		auto begin()noexcept
		{
			return m_pipelines.begin();
		}

		auto begin()const noexcept
		{
			return m_pipelines.begin();
		}

		auto end()noexcept
		{
			return m_pipelines.end();
		}

		auto end()const noexcept
		{
			return m_pipelines.end();
		}

		auto size()const noexcept
		{
			return m_pipelines.size();
		}

		auto empty()const noexcept
		{
			return m_pipelines.empty();
		}

	private:
		std::map< uint32_t, PipelineNodes > m_pipelines;
	};
}

#endif
