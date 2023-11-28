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
		using NodesView = NodesViewT< NodeT, 1024u >;

		static uint64_t constexpr maxObjects = 512ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxObjects;

		using CountedNode = CountedNodeT< NodeT >;
		using NodeObject = NodeObjectT< NodeT >;

		struct ObjectNodes
		{
			NodeObject const * object{};
			NodesView nodes;
		};

		explicit InstantiatedObjectsNodesViewT( CountedNode * data )
			: m_objects{ maxObjects }
			, m_count{}
		{
			if ( data )
			{
				for ( auto & buffer : m_objects )
				{
					buffer.nodes = NodesView{ data };
					data += NodesView::maxCount;
				}
			}
		}

		InstantiatedObjectsNodesViewT()
			: InstantiatedObjectsNodesViewT{ nullptr }
		{
		}

		auto emplace( NodeObject const & object )
		{
			auto it = std::find_if( begin()
				, end()
				, [&object]( ObjectNodes const & lookup )
				{
					return lookup.object == &object;
				} );

			if ( it == end() )
			{
				CU_Assert( m_count < maxObjects
					, "Too many objects for given pass, buffer and pipeline" );
#if C3D_EnsureNodesCounts
				if ( m_count == maxObjects )
				{
					CU_Exception( "Too many objects for given pass, buffer and pipeline" );
				}
#endif

				it->object = &object;
				++m_count;
			}

			return it;
		}

		void emplace( NodeObject const & object
			, CountedNode & node )
		{
			auto it = emplace( object );
			it->nodes.emplace( node );
			CU_Require( !it->nodes.empty() );
		}

		void clear()noexcept
		{
			for ( auto & [object, nodes] : m_objects )
			{
				object = {};
				nodes.clear();
			}

			m_count = 0u;
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
		std::vector< ObjectNodes > m_objects;
		uint32_t m_count;
	};

	template< typename NodeT >
	struct InstantiatedPassesNodesViewT
	{
		static uint64_t constexpr maxPasses = 64ull;
		static uint64_t constexpr maxCount = InstantiatedObjectsNodesViewT< NodeT >::maxCount * maxPasses;

		using CountedNode = CountedNodeT< NodeT >;
		using InstantiatedObjectsNodesView = InstantiatedObjectsNodesViewT< NodeT >;
		using NodeObject = NodeObjectT< NodeT >;

		struct PassNodes
		{
			Pass const * pass{};
			InstantiatedObjectsNodesView objects;
		};

		explicit InstantiatedPassesNodesViewT( CountedNode * data )
			: m_passes{ maxPasses }
			, m_count{}
		{
			if ( data )
			{
				for ( auto & pass : m_passes )
				{
					pass.objects = InstantiatedObjectsNodesView{ data };
					data += InstantiatedObjectsNodesView::maxCount;
				}
			}
		}

		InstantiatedPassesNodesViewT()
			: InstantiatedPassesNodesViewT{ nullptr }
		{
		}

		auto emplace( Pass const & pass )
		{
			auto it = std::find_if( begin()
				, end()
				, [&pass]( PassNodes const & lookup )
				{
					return lookup.pass == &pass;
				} );

			if ( it == end() )
			{
				CU_Assert( m_count < maxPasses
					, "Too many passes for given buffer and pipeline" );
#if C3D_EnsureNodesCounts
				if ( m_count == maxPasses )
				{
					CU_Exception( "Too many passes for given buffer and pipeline" );
				}
#endif

				it->pass = &pass;
				++m_count;
			}

			return it;
		}

		void emplace( Pass const & pass
			, NodeObject const & object
			, CountedNode & node )
		{
			auto it = emplace( pass );
			it->objects.emplace( object, node );
			CU_Require( !it->objects.empty() );
		}

		void clear()noexcept
		{
			for ( auto & [pass, objects] : m_passes )
			{
				pass = {};
				objects.clear();
			}

			m_count = 0u;
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
		std::vector< PassNodes > m_passes;
		uint32_t m_count;
	};

	template< typename NodeT >
	struct InstantiatedBuffersNodesViewT
	{
		static uint64_t constexpr maxBuffers = 16ull;
		static uint64_t constexpr maxCount = InstantiatedObjectsNodesViewT< NodeT >::maxCount * maxBuffers;

		using CountedNode = CountedNodeT< NodeT >;
		using NodeObject = NodeObjectT< NodeT >;
		using NodeArray = NodeArrayT< NodeT >;
		using InstantiatedObjectsNodesView = InstantiatedObjectsNodesViewT< NodeT >;

		struct BufferNodes
		{
			ashes::BufferBase const * buffer{};
			InstantiatedObjectsNodesView objects;
		};

		explicit InstantiatedBuffersNodesViewT()
			: m_nodes{ maxCount }
			, m_nodeCount{}
			, m_buffers{ maxBuffers }
			, m_count{}
		{
			CountedNode * data = m_nodes.data();

			for ( auto & buffer : m_buffers )
			{
				buffer.objects = InstantiatedObjectsNodesView{ data };
				data += InstantiatedObjectsNodesView::maxCount;
			}
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
			, Pass const & pass
			, NodeObject const & object
			, CountedNode const & node
			, uint32_t drawCount
			, bool isFrontCulled )
		{
			auto ires = m_countedNodes.emplace( node.node, nullptr );

			if ( ires.second )
			{
				CU_Assert( m_nodeCount < maxCount
					, "Too many nodes for given pipeline and buffer" );
#if C3D_EnsureNodesCounts
				if ( m_nodeCount == maxCount )
				{
					CU_Exception( "Too many nodes for given pipeline and buffer" );
				}
#endif
				m_nodes[m_nodeCount] = node;
				ires.first->second = &m_nodes[m_nodeCount];
				++m_nodeCount;

				auto it = emplace( buffer );
				it->objects.emplace( object, *ires.first->second );
				CU_Require( !it->objects.empty() );
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
			for ( auto & [buffer, objects] : m_buffers )
			{
				buffer = {};
				objects.clear();
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
		NodeArray m_nodes;
		uint32_t m_nodeCount;
		std::unordered_map< NodeT const *, CountedNode * > m_countedNodes;
		std::vector< BufferNodes > m_buffers;
		uint32_t m_count;
	};

	template< typename NodeT >
	class InstantiatedPipelinesNodesT
	{
	public:
		static uint64_t constexpr maxPipelines = 256ull;
		static uint64_t constexpr maxCount = InstantiatedBuffersNodesViewT< NodeT >::maxCount * maxPipelines;

		using CountedNode = CountedNodeT< NodeT >;
		using NodeObject = NodeObjectT< NodeT >;
		using InstantiatedBuffersNodesView = InstantiatedBuffersNodesViewT< NodeT >;

		struct PipelineNodes
		{
			PipelineAndID pipeline{};
			bool isFrontCulled{};
			InstantiatedBuffersNodesView buffers{};
		};

		InstantiatedPipelinesNodesT()
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

				it = m_pipelines.emplace( id, PipelineNodes{ pipeline, isFrontCulled, InstantiatedBuffersNodesView{} } ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, ashes::BufferBase const & buffer
			, Pass const & pass
			, NodeObject const & object
			, CountedNode const & node
			, uint32_t drawCount
			, bool isFrontCulled )
		{
			auto it = emplace( pipeline, isFrontCulled );
			it->buffers.emplace( buffer, pass, object, node , drawCount, isFrontCulled );
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

	template< typename NodeT >
	class InstantiatedPipelinesDrawnNodesT
	{
	public:
		static uint64_t constexpr maxPipelines = 256ull;
		static uint64_t constexpr maxCount = InstantiatedBuffersNodesViewT< NodeT >::maxCount * maxPipelines;

		using CountedNode = CountedNodeT< NodeT >;
		using InstantiatedBuffersNodesView = InstantiatedBuffersNodesViewT< NodeT >;
		using NodeArray = NodeArrayT< NodeT >;

		struct PipelineNodes
		{
			PipelineAndID pipeline{};
			bool isFrontCulled{};
			InstantiatedBuffersNodesView buffers;
		};

		InstantiatedPipelinesDrawnNodesT()
		{
		}

		auto emplace( PipelineAndID const & pipeline
			, bool isFrontCulled )
		{
			auto id = uint32_t( pipeline.id + ( isFrontCulled ? ( maxPipelines / 2u ) : 0u ) );
			auto it = m_pipelines.find( id );

			if ( it == m_pipelines.end() )
			{
				it = m_pipelines.emplace( id, PipelineNodes{ pipeline, isFrontCulled, InstantiatedBuffersNodesView{} } ).first;
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
