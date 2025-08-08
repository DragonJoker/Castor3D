/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelinesNodes_H___
#define ___C3D_PipelinesNodes_H___

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace c3d
{
	static bool constexpr C3D_EnsureNodesCounts = true;

	template< typename NodeT >
	struct RenderedNodeT
	{
		RenderedNodeT() = default;

		RenderedNodeT( NodeT const * node
			, bool visible
			, NodeCommandT< NodeT > command )
			: node{ node }
			, visible{ visible }
			, command{ c3d::move( command ) }
		{
		}

		NodeT const * node{};
		bool visible{};
		NodeCommandT< NodeT > command{};
	};

	template< typename NodeT >
	struct NodesViewT
	{
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodeArray = NodeArrayT< NodeT, RenderedNodeT >;

		static uint64_t constexpr maxNodes = 1024ULL;
		static uint64_t constexpr maxCount = maxNodes;

		RenderedNode * emplace( RenderedNode node )
		{
			CU_Assert( size() < maxNodes
				, "Too many nodes for given buffer and given pipeline" );

			if constexpr ( C3D_EnsureNodesCounts )
			{
				if ( size() == maxNodes )
				{
					CU_Exception( "Too many nodes for given buffer and given pipeline" );
				}
			}

			m_nodes[m_count] = c3d::move( node );
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
		NodeArray m_nodes{ maxCount };
		size_t m_count{};
	};

	template< typename NodeT >
	struct BuffersNodesViewT
	{
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodesView = NodesViewT< NodeT >;

		static uint64_t constexpr maxBuffers = 32ULL;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxBuffers;

		struct BufferNodes
		{
			explicit BufferNodes( BufferBase const * posBuffer = {}
				, BufferBase const * idxBuffer = {} )
				: posBuffer{ posBuffer }
				, idxBuffer{ idxBuffer }
			{
			}

			BufferBase const * posBuffer{};
			BufferBase const * idxBuffer{};
			NodesView nodes{};
		};

		auto emplace( BufferBase const & posBuffer
			, BufferBase const * idxBuffer )
		{
			auto it = std::find_if( begin()
				, end()
				, [&posBuffer, idxBuffer]( BufferNodes const & lookup )
				{
					return lookup.posBuffer == &posBuffer
						&& lookup.idxBuffer == idxBuffer;
				} );

			if ( it == end() )
			{
				CU_Assert( size() < maxBuffers
					, "Too many buffers for given pipeline" );

				if constexpr ( C3D_EnsureNodesCounts )
				{
					if ( size() == maxBuffers )
					{
						CU_Exception( "Too many buffers for given pipeline" );
					}
				}

				m_buffers.emplace_back( &posBuffer, idxBuffer );
				it = std::next( m_buffers.begin(), ptrdiff_t( m_buffers.size() ) - 1 );
			}

			return it;
		}

		RenderedNode * emplace( BufferBase const & posBuffer
			, BufferBase const * idxBuffer
			, RenderedNode node )
		{
			auto it = emplace( posBuffer, idxBuffer );
			return it->nodes.emplace( c3d::move( node ) );
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
		Vector< BufferNodes > m_buffers{};
	};

	template< typename NodeT >
	class PipelinesNodesT
	{
	public:
		using CulledNode = CulledNodeT< NodeT >;
		using NodeCommand = NodeCommandT< NodeT >;
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodesView = BuffersNodesViewT< NodeT >;

		static uint64_t constexpr maxPipelines = 128ULL;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxPipelines;

		struct PipelineNodes
		{
			PipelineNodes( PipelineAndID pipeline
				, bool isFrontCulled )
				: pipeline{ c3d::move( pipeline ) }
				, isFrontCulled{ isFrontCulled }
			{
			}

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

				if constexpr ( C3D_EnsureNodesCounts )
				{
					if ( pipeline.id >= maxPipelines / 2u )
					{
						CU_Exception( "Too many pipelines" );
					}
				}

				it = m_pipelines.try_emplace( id, pipeline, isFrontCulled ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, BufferBase const & posBuffer
			, BufferBase const * idxBuffer
			, CulledNode const & culled
			, NodeCommand command
			, bool isFrontCulled )
		{
			size_t hash = std::hash< NodeT const * >{}( culled.node );
			hash = hashCombine( hash, isFrontCulled );
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
				ires.first->second = it->nodes.emplace( posBuffer
					, idxBuffer
					, RenderedNode{ culled.node
						, culled.visible
						, c3d::move( command ) } );
			}
			else
			{
				ires.first->second->visible = culled.visible;
				ires.first->second->command = c3d::move( command );
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
		Map< uint32_t, PipelineNodes > m_pipelines;
		HashMap< size_t, RenderedNode * > m_countedNodes;
	};
}

#endif
