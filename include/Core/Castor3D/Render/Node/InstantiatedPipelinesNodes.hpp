/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiatedPipelinesNodes_H___
#define ___C3D_InstantiatedPipelinesNodes_H___

#include "Castor3D/Render/Node/PipelinesNodes.hpp"

namespace castor3d
{
	template< typename NodeT >
	struct InstantiatedObjectsNodesViewT
	{
		using NodeObject = NodeObjectT< NodeT >;
		using NodeInstance = NodeInstanceT< NodeT >;
		using RenderedNode = RenderedNodeT< NodeT >;
		using NodeInstances = castor::Pair< RenderedNode, castor::UnorderedSet< NodeInstance const * > >;

		static uint64_t constexpr maxObjects = 1024ULL;
		static uint64_t constexpr maxCount = maxObjects;

		auto emplace( RenderedNode node )
		{
			auto data = &node.node->data;
			auto it = std::find_if( begin()
				, end()
				, [&data]( castor::Pair< NodeObject const *, NodeInstances > const & lookup )
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

				NodeInstances instances{ castor::move( node ), {} };
				m_objects.emplace_back( data, castor::move( instances ) );
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
		castor::Vector< castor::Pair< NodeObject const *, NodeInstances > > m_objects;
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
			explicit BufferNodes( ashes::BufferBase const * buffer )
				: buffer{ buffer }
			{
			}

			ashes::BufferBase const * buffer{};
			NodesView nodes;
		};

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

				if constexpr ( C3D_EnsureNodesCounts )
				{
					if ( size() == maxBuffers )
					{
						CU_Exception( "Too many buffers for given pipeline" );
					}
				}

				m_buffers.emplace_back( &buffer );
				it = std::next( begin(), ptrdiff_t( size() - 1u ) );
			}

			return it;
		}

		void emplace( ashes::BufferBase const & buffer
			, RenderedNode node )
		{
			auto it = emplace( buffer );
			it->nodes.emplace( castor::move( node ) );
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
		castor::Vector< BufferNodes > m_buffers;
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
				: pipeline{ castor::move( pipeline ) }
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
			, ashes::BufferBase const & buffer
			, CulledNode const & culled
			, NodeCommand command
			, bool isFrontCulled )
		{
			auto & node = *culled.node;
			size_t hash = std::hash< NodeObject const * >{}( &node.data );
			hash = castor::hashCombine( hash, node.pass->getHash() );
			hash = castor::hashCombine( hash, isFrontCulled );

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
			it->nodes.emplace( buffer
				, RenderedNode{ culled.node
					, culled.visible
					, castor::move( command ) } );
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
		castor::UnorderedSet< size_t > m_countedNodes;
		castor::Map< uint32_t, PipelineNodes > m_pipelines;
	};
}

#endif
