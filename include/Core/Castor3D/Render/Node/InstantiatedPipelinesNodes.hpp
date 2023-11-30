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

		static uint64_t constexpr maxObjects = 1024ull;
		static uint64_t constexpr maxCount = maxObjects;

		auto emplace( NodeT const & node )
		{
			auto it = std::find_if( begin()
				, end()
				, [&node]( std::pair< NodeObject const *, NodeT const * > const & lookup )
				{
					return lookup.first == &node.data;
				} );

			if ( it == end() )
			{
				CU_Assert( size() < maxObjects
					, "Too many objects for given pass, buffer and pipeline" );
#if C3D_EnsureNodesCounts
				if ( size() == maxObjects )
				{
					CU_Exception( "Too many objects for given pass, buffer and pipeline" );
				}
#endif

				m_objects.emplace_back( &node.data, &node );
				it = std::next( begin(), ptrdiff_t( size() - 1u ) );
			}

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
			return size() * ( sizeof( NodeObject * ) + sizeof( NodeT * ) );
		}

	private:
		std::vector< std::pair< NodeObject const *, NodeT const * > > m_objects;
	};

	template< typename NodeT >
	struct InstantiatedPassesNodesViewT
	{
		using NodesView = InstantiatedObjectsNodesViewT< NodeT >;

		static uint64_t constexpr maxPasses = 512ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxPasses;

		struct PassNodes
		{
			uint32_t passId{};
			NodesView nodes;
		};

		auto emplace( Pass const & pass )
		{
			auto it = std::find_if( begin()
				, end()
				, [&pass]( PassNodes const & lookup )
				{
					return lookup.passId == pass.getHash();
				} );

			if ( it == end() )
			{
				CU_Assert( size() < maxPasses
					, "Too many passes for given buffer and pipeline" );
#if C3D_EnsureNodesCounts
				if ( size() == maxPasses )
				{
					CU_Exception( "Too many passes for given buffer and pipeline" );
				}
#endif

				m_passes.push_back( PassNodes{ pass.getHash(), NodesView{} } );
				it = std::next( begin(), ptrdiff_t( size() - 1u ) );
			}

			return it;
		}

		void emplace( NodeT const & node )
		{
			auto it = emplace( *node.pass );
			it->nodes.emplace( node );
		}

		void clear()noexcept
		{
			m_passes.clear();
		}

		auto begin()noexcept
		{
			return m_passes.begin();
		}

		auto begin()const noexcept
		{
			return m_passes.begin();
		}

		auto end()noexcept
		{
			return m_passes.end();
		}

		auto end()const noexcept
		{
			return m_passes.end();
		}

		auto size()const noexcept
		{
			return m_passes.size();
		}

		auto empty()const noexcept
		{
			return m_passes.empty();
		}

		size_t occupancy()const noexcept
		{
			size_t result = size() * ( sizeof( PassNodes ) );

			for ( auto & pass : m_passes )
			{
				result += pass.nodes.occupancy();
			}

			return result;
		}

	private:
		std::vector< PassNodes > m_passes;
	};

	template< typename NodeT >
	struct InstantiatedBuffersNodesViewT
	{
		using NodesView = InstantiatedPassesNodesViewT< NodeT >;

		static uint64_t constexpr maxBuffers = BuffersNodesViewT< NodeT >::maxBuffers;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxBuffers;

		struct BufferNodes
		{
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
#if C3D_EnsureNodesCounts
				if ( size() == maxBuffers )
				{
					CU_Exception( "Too many buffers for given pipeline" );
				}
#endif

				m_buffers.push_back( BufferNodes{ &buffer, NodesView{} } );
				it = std::next( begin(), ptrdiff_t( size() - 1u ) );
			}

			return it;
		}

		void emplace( ashes::BufferBase const & buffer
			, NodeT const & node )
		{
			auto it = emplace( buffer );
			it->nodes.emplace( node );
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
		std::vector< BufferNodes > m_buffers;
	};

	template< typename NodeT >
	class InstantiatedPipelinesNodesT
	{
	public:
		using NodeObject = NodeObjectT< NodeT >;
		using CountedNode = CountedNodeT< NodeT >;
		using NodesView = InstantiatedBuffersNodesViewT< NodeT >;

		static uint64_t constexpr maxPipelines = PipelinesNodesT< NodeT >::maxPipelines;
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
					, PipelineNodes{ pipeline, isFrontCulled, NodesView{} } ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, ashes::BufferBase const & buffer
			, NodeT const & node
			, CountedNode culled
			, uint32_t drawCount
			, bool isFrontCulled )
		{
			size_t hash = std::hash< NodeObject const * >{}( &node.data );
			hash = castor::hashCombine( hash, node.pass->getHash() );
			hash = castor::hashCombine( hash, isFrontCulled );
			auto ires = m_countedNodes.emplace( hash );

			if ( ires.second )
			{
				CU_Assert( m_countedNodes.size() < maxCount
					, "Too many nodes" );
#if C3D_EnsureNodesCounts
				if ( m_countedNodes.size() == maxCount )
				{
					CU_Exception( "Too many nodes" );
				}
#endif
				auto it = emplace( pipeline, isFrontCulled );
				it->nodes.emplace( buffer, node );
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
		std::unordered_set< size_t > m_countedNodes;
		std::map< uint32_t, PipelineNodes > m_pipelines;
	};
}

#endif
