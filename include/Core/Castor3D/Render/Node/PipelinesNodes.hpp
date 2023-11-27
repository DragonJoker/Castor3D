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
	template< typename NodeT >
	struct NodesViewT
	{
		static uint64_t constexpr maxNodes = 4'096ull;
		static uint64_t constexpr maxCount = maxNodes;

		using CountedNode = CountedNodeT< NodeT >;

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
			CU_Assert( m_count < maxNodes
				, "Too many nodes for given buffer and given pipeline" );
#if C3D_EnsureNodesCounts
			if ( m_count == maxNodes )
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
		static uint64_t constexpr maxBuffers = 16ull;
		static uint64_t constexpr maxCount = NodesViewT< NodeT >::maxCount * maxBuffers;

		using CountedNode = CountedNodeT< NodeT >;
		using NodesView = NodesViewT< NodeT >;

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
		static uint64_t constexpr maxPipelines = 256ull;
		static uint64_t constexpr maxCount = BuffersNodesViewT< NodeT >::maxCount * maxPipelines;

		using CountedNode = CountedNodeT< NodeT >;
		using BuffersNodesView = BuffersNodesViewT< NodeT >;
		using NodeArray = NodeArrayT< NodeT >;

		struct PipelineNodes
		{
			PipelineAndID pipeline{};
			bool isFrontCulled{};
			BuffersNodesView buffers;
		};

		PipelinesNodesT()
			: m_nodes{ maxCount }
			, m_nodeCount{}
			, m_pipelines{ maxPipelines }
		{
			auto data = m_nodes.data();

			for ( auto & pipeline : m_pipelines )
			{
				pipeline.buffers = BuffersNodesView{ data };
				data += BuffersNodesView::maxCount;
			}
		}

		auto emplace( PipelineAndID const & pipeline
			, bool isFrontCulled )
		{
			CU_Assert( pipeline.id < maxPipelines
				, "Too many pipelines" );
#if C3D_EnsureNodesCounts
			if ( pipeline.id == maxPipelines / 2u )
			{
				CU_Exception( "Too many pipelines" );
			}
#endif

			auto & result = m_pipelines[pipeline.id + ( isFrontCulled ? ( maxPipelines / 2u ) : 0u )];
			result.pipeline = pipeline;
			result.isFrontCulled = isFrontCulled;
			return &result;
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
				it->buffers.emplace( buffer, *ires.first->second );
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
			for ( auto & [pipeline, isFrontCulled, buffers] : m_pipelines )
			{
				pipeline.id = {};
				pipeline.pipeline = {};
				buffers.clear();
			}
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
		std::vector< PipelineNodes > m_pipelines;
		std::unordered_map< NodeT const *, CountedNode * > m_countedNodes;
	};
}

#endif
