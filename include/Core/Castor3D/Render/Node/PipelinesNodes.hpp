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
		NodeT const * node{};
		bool visible{};
		NodeCommandT< NodeT > command{};
	};

	template< typename NodeT >
	struct NodesViewT
	{
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodeArray = NodeArrayT< NodeT, RenderedNodeT >;

		static uint64_t constexpr maxNodes = 1024;
		static uint64_t constexpr maxCount = maxNodes;

		explicit NodesViewT()
			: m_nodes{ maxCount }
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
		NodeArray m_nodes;
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
			NodesView nodes{};
		};

		explicit BuffersNodesViewT()
			: m_buffers{}
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

				m_buffers.push_back( BufferNodes{ &buffer, NodesView{} } );
				it = std::next( m_buffers.begin(), ptrdiff_t( m_buffers.size() ) - 1 );
			}

			return it;
		}

		RenderedNode * emplace( ashes::BufferBase const & buffer
			, RenderedNode node )
		{
			auto it = emplace( buffer );
			return it->nodes.emplace( std::move( node ) );
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
			return m_buffers.end();
		}

		auto end()const noexcept
		{
			return m_buffers.end();
		}

		auto size()const noexcept
		{
			return m_buffers.size();
		}

		auto empty()const noexcept
		{
			return m_buffers.empty();
		}

		size_t occupancy()const noexcept
		{
			return sizeof( *this )
				+ m_buffers.size() * ( sizeof( BufferNodes ) + NodesView::maxCount * sizeof( RenderedNode ) );
		}

	private:
		std::vector< BufferNodes > m_buffers;
	};

	template< typename NodeT >
	class PipelinesNodesT
	{
	public:
		using CulledNode = CulledNodeT< NodeT >;
		using NodeCommand = NodeCommandT< NodeT >;
		using RenderedNode = RenderedNodeT< NodeT >;
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

				it = m_pipelines.emplace( id
					, PipelineNodes{ pipeline
						, isFrontCulled
						, NodesView{} } ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, ashes::BufferBase const & buffer
			, CulledNode const & culled
			, NodeCommand command
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
					, RenderedNode{ culled.node
						, culled.visible
						, std::move( command ) } );
			}
			else
			{
				ires.first->second->visible = culled.visible;
			}
		}

		void clear()noexcept
		{
			m_pipelines.clear();
			m_countedNodes.clear();
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
			size_t result{ sizeof( *this ) };

			for ( auto & pipeline : m_pipelines )
			{
				result += sizeof( PipelineNodes ) + pipeline.second.nodes.occupancy();
			}

			return result;
		}

	private:
		std::map< uint32_t, PipelineNodes > m_pipelines;
		std::unordered_map< size_t, RenderedNode * > m_countedNodes;
	};
}

#endif
