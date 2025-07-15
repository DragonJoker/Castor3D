/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiatedPipelinesNodes_H___
#define ___C3D_InstantiatedPipelinesNodes_H___

#include "Castor3D/Render/Node/PipelinesNodes.hpp"

namespace c3d
{
	template< typename NodeT >
	struct InstantiatedObjectsNodesViewT
	{
		using NodeObject = NodeObjectT< NodeT >;
		using NodeInstance = NodeInstanceT< NodeT >;
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodeInstances = Pair< RenderedNode, HashSet< NodeInstance const * > >;

		static uint64_t constexpr maxObjects = 1024ULL;
		static uint64_t constexpr maxCount = maxObjects;

		auto emplace( RenderedNode node )
		{
			auto data = &node.node->data;
			auto it = std::find_if( begin()
				, end()
				, [&data]( Pair< NodeObject const *, NodeInstances > const & lookup )
				{
					return lookup.first == data;
				} );
			auto instance = &node.node->instance;

			if ( it == end() )
			{
				CU_Assert( size() < maxObjects
					, "Too many objects for given pass, buffer and pipeline" );

				if constexpr ( C3D_EnsureNodesCounts )
				{
					if ( size() == maxObjects )
					{
						CU_Exception( "Too many objects for given pass, buffer and pipeline" );
					}
				}

				NodeInstances instances{ c3d::move( node ), {} };
				m_objects.emplace_back( data, c3d::move( instances ) );
				it = std::next( begin(), ptrdiff_t( size() - 1u ) );
			}

			it->second.second.emplace( instance );
			return it;
		}

		void clear()noexcept
		{
			m_objects.clear();
		}

		auto begin()noexcept
		{
			return m_objects.begin();
		}

		auto begin()const noexcept
		{
			return m_objects.begin();
		}

		auto end()noexcept
		{
			return m_objects.end();
		}

		auto end()const noexcept
		{
			return m_objects.end();
		}

		auto size()const noexcept
		{
			return m_objects.size();
		}

		auto empty()const noexcept
		{
			return m_objects.empty();
		}

		size_t occupancy()const noexcept
		{
			return size() * ( sizeof( NodeObject * ) + sizeof( RenderedNode ) );
		}

	private:
		Vector< Pair< NodeObject const *, NodeInstances > > m_objects;
	};

	template< typename NodeT >
	struct InstantiatedBuffersNodesViewT
	{
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodesView = InstantiatedObjectsNodesViewT< NodeT >;

		static uint64_t constexpr maxBuffers = BuffersNodesViewT< NodeT >::maxBuffers;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxBuffers;

		struct BufferNodes
		{
			explicit BufferNodes( ashes::BufferBase const * posBuffer
				, ashes::BufferBase const * idxBuffer )
				: posBuffer{ posBuffer }
				, idxBuffer{ idxBuffer }
			{
			}

			ashes::BufferBase const * posBuffer{};
			ashes::BufferBase const * idxBuffer{};
			NodesView nodes;
		};

		auto emplace( ashes::BufferBase const & posBuffer
			, ashes::BufferBase const * idxBuffer )
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
				it = std::next( begin(), ptrdiff_t( size() - 1u ) );
			}

			return it;
		}

		void emplace( ashes::BufferBase const & posBuffer
			, ashes::BufferBase const * idxBuffer
			, RenderedNode node )
		{
			auto it = emplace( posBuffer, idxBuffer );
			it->nodes.emplace( c3d::move( node ) );
		}

		void clear()noexcept
		{
			m_buffers.clear();
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
			size_t result = size() * ( sizeof( BufferNodes ) );

			for ( auto & buffer : m_buffers )
			{
				result += buffer.nodes.occupancy();
			}

			return result;
		}

	private:
		Vector< BufferNodes > m_buffers;
	};

	template< typename NodeT >
	class InstantiatedPipelinesNodesT
	{
	public:
		using NodeObject = NodeObjectT< NodeT >;
		using CulledNode = CulledNodeT< NodeT >;
		using NodeCommand = NodeCommandT< NodeT >;
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodesView = InstantiatedBuffersNodesViewT< NodeT >;

		static uint64_t constexpr maxPipelines = PipelinesNodesT< NodeT >::maxPipelines;
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
			, ashes::BufferBase const & posBuffer
			, ashes::BufferBase const * idxBuffer
			, CulledNode const & culled
			, NodeCommand command
			, bool isFrontCulled )
		{
			auto & node = *culled.node;
			size_t hash = std::hash< NodeObject const * >{}( &node.data );
			hash = hashCombine( hash, node.pass->getHash() );
			hash = hashCombine( hash, isFrontCulled );

			if ( m_countedNodes.emplace( hash ).second )
			{
				CU_Assert( m_countedNodes.size() < maxCount
					, "Too many nodes" );
#if C3D_EnsureNodesCounts
				if ( m_countedNodes.size() == maxCount )
				{
					CU_Exception( "Too many nodes" );
				}
#endif
			}

			auto it = emplace( pipeline, isFrontCulled );
			it->nodes.emplace( posBuffer
				, idxBuffer
				, RenderedNode{ culled.node
					, culled.visible
					, c3d::move( command ) } );
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

		auto empty()const noexcept
		{
			return m_pipelines.empty();
		}

		size_t occupancy()const noexcept
		{
			size_t result = m_pipelines.size() * ( sizeof( PipelineNodes ) );

			for ( auto & pipeline : m_pipelines )
			{
				result += pipeline.second.nodes.occupancy();
			}

			return result;
		}

	private:
		HashSet< size_t > m_countedNodes;
		Map< uint32_t, PipelineNodes > m_pipelines;
	};
}

#endif
