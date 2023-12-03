/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelinesNodes_H___
#define ___C3D_PipelinesNodes_H___

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#define C3D_EnsureNodesCounts 1

namespace castor3d
{
	template< typename NodeT >
	struct RenderedNodeT
	{
		CulledNodeT< NodeT > const * culled;
		GeometryBuffers const * geometryBuffers;
		uint32_t idxvtxCount;
		uint32_t firstIndex;
		uint32_t vertexOffset;
	};

	template< typename NodeT >
	struct NodesViewT
	{
		using RenderedNode = RenderedNodeT< NodeT >;

		static uint64_t constexpr maxNodes = 1024;
		static uint64_t constexpr maxCount = maxNodes;

		explicit NodesViewT( castor::ArrayView< RenderedNode > data )
			: m_nodes{ std::move( data ) }
			, m_count{}
		{
		}

		NodesViewT()
			: m_nodes{}
			, m_count{}
		{
		}

		RenderedNode * emplace( RenderedNode node )
		{
			CU_Assert( size() < maxNodes
				, "Too many nodes for given buffer and given pipeline" );
#if C3D_EnsureNodesCounts
			if ( size() == maxNodes )
			{
				CU_Exception( "Too many nodes for given buffer and given pipeline" );
			}
#endif

			m_nodes[m_count] = std::move( node );
			return &m_nodes[m_count++];
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
		castor::ArrayView< RenderedNode > m_nodes;
		size_t m_count;
	};

	template< typename NodeT >
	struct BuffersNodesViewT
	{
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodesView = NodesViewT< NodeT >;

		static uint64_t constexpr maxBuffers = 32ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxBuffers;

		struct BufferNodes
		{
			ashes::BufferBase const * buffer{};
			NodesView nodes;
		};

		explicit BuffersNodesViewT( castor::ArrayView< RenderedNode > data )
			: m_buffers{ maxBuffers }
			, m_count{}
		{
			if ( !data.empty() )
			{
				auto index = 0u;

				for ( auto & buffer : m_buffers )
				{
					buffer.nodes = NodesView{ castor::makeArrayView( data.begin() + index, NodesView::maxCount ) };
					index += NodesView::maxCount;
				}
			}
		}

		BuffersNodesViewT()
			: BuffersNodesViewT{ castor::ArrayView< RenderedNode >{} }
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
				CU_Assert( size() < maxBuffers
					, "Too many buffers for given pipeline" );
#if C3D_EnsureNodesCounts
				if ( size() == maxBuffers )
				{
					CU_Exception( "Too many buffers for given pipeline" );
				}
#endif

				it->buffer = &buffer;
				++m_count;
			}

			return it;
		}

		RenderedNode * emplace( ashes::BufferBase const & buffer
			, RenderedNode node )
		{
			auto it = emplace( buffer );
			return it->nodes.emplace( std::move( node ) );
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

		static constexpr size_t occupancy()noexcept
		{
			return maxBuffers * sizeof( BufferNodes );
		}

	private:
		std::vector< BufferNodes > m_buffers;
		uint32_t m_count;
	};

	template< typename NodeT >
	class PipelinesNodesT
	{
	public:
		using CulledNode = CulledNodeT< NodeT >;
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodeArray = NodeArrayT< NodeT, RenderedNodeT >;
		using NodesView = BuffersNodesViewT< NodeT >;

		static uint64_t constexpr maxPipelines = 128ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxPipelines;

		struct PipelineNodes
		{
			PipelineAndID pipeline{};
			bool isFrontCulled{};
			NodesView nodes{};
		};

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

				if ( m_nodes.empty() )
				{
					m_nodes.resize( maxCount );
				}

				it = m_pipelines.emplace( id
					, PipelineNodes{ pipeline
						, isFrontCulled
						, NodesView{ castor::makeArrayView( m_nodes.data() + ( m_pipelines.size() * NodesView::maxCount )
							, NodesView::maxCount ) } } ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, ashes::BufferBase const & buffer
			, NodeT const & node
			, CulledNode const & culled
			, GeometryBuffers const & geometryBuffers
			, uint32_t count
			, uint32_t firstIndex
			, uint32_t vertexOffset
			, bool isFrontCulled )
		{
			size_t hash = std::hash< NodeT const * >{}( culled.node );
			hash = castor::hashCombine( hash, isFrontCulled );
			auto ires = m_countedNodes.emplace( hash, nullptr );

			if ( ires.second )
			{
				CU_Assert( nodesCount() <= maxCount
					, "Too many nodes" );
#if C3D_EnsureNodesCounts
				if ( nodesCount() > maxCount )
				{
					CU_Exception( "Too many nodes" );
				}
#endif
				auto it = emplace( pipeline, isFrontCulled );
				ires.first->second = it->nodes.emplace( buffer
					, RenderedNode{ &culled
						, &geometryBuffers
						, count
						, firstIndex
						, vertexOffset } );
			}
		}

		void clear()noexcept
		{
			m_pipelines.clear();
			m_countedNodes.clear();
			m_nodes.clear();
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
			return m_countedNodes.size();
		}

		auto empty()const noexcept
		{
			return m_pipelines.empty();
		}

		size_t occupancy()const noexcept
		{
			return maxCount * sizeof( RenderedNode )
				+ m_pipelines.size() * ( sizeof( PipelineNodes ) + NodesView::occupancy() );
		}

	private:
		NodeArray m_nodes;
		std::map< uint32_t, PipelineNodes > m_pipelines;
		std::unordered_map< size_t, RenderedNode * > m_countedNodes;
	};
}

#endif
