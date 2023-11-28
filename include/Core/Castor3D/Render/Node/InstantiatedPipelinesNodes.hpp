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
		static uint64_t constexpr maxObjects = 64ull;
		static uint64_t constexpr maxCount = maxObjects;

		explicit InstantiatedObjectsNodesViewT()
			: m_objects{ maxObjects }
			, m_count{}
		{
		}

		auto emplace( NodeT const & object )
		{
			auto it = std::find_if( begin()
				, end()
				, [&object]( NodeT const * lookup )
				{
					return lookup == &object;
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

				*it = &object;
				++m_count;
			}

			return it;
		}

		void clear()noexcept
		{
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
		std::vector< NodeT const * > m_objects;
		uint32_t m_count;
	};

	template< typename NodeT >
	struct InstantiatedPassesNodesViewT
	{
		using NodesView = InstantiatedObjectsNodesViewT< NodeT >;

		static uint64_t constexpr maxPasses = 64ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxPasses;

		struct PassNodes
		{
			Pass const * pass{};
			NodesView nodes;
		};

		explicit InstantiatedPassesNodesViewT()
			: m_passes{ maxPasses }
			, m_count{}
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
			, NodeT const & object )
		{
			auto it = emplace( pass );
			it->nodes.emplace( object );
		}

		void clear()noexcept
		{
			for ( auto & [pass, nodes] : m_passes )
			{
				pass = {};
				nodes.clear();
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
		using NodesView = InstantiatedPassesNodesViewT< NodeT >;

		static uint64_t constexpr maxBuffers = 16ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxBuffers;

		struct BufferNodes
		{
			ashes::BufferBase const * buffer{};
			NodesView nodes;
		};

		explicit InstantiatedBuffersNodesViewT()
			: m_buffers{ maxBuffers }
			, m_count{}
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
			, Pass const & pass
			, NodeT const & object )
		{
			auto it = emplace( buffer );
			it->nodes.emplace( pass, object );
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
	class InstantiatedPipelinesNodesT
	{
	public:
		using NodesView = InstantiatedBuffersNodesViewT< NodeT >;

		static uint64_t constexpr maxPipelines = 256ull;
		static uint64_t constexpr maxCount = NodesView::maxCount * maxPipelines;

		struct PipelineNodes
		{
			PipelineAndID pipeline{};
			bool isFrontCulled{};
			NodesView nodes{};
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
				it = m_pipelines.emplace( id
					, PipelineNodes{ pipeline, isFrontCulled, NodesView{} } ).first;
			}

			return &it->second;
		}

		void emplace( PipelineAndID const & pipeline
			, ashes::BufferBase const & buffer
			, Pass const & pass
			, NodeT const & object
			, bool isFrontCulled )
		{
			auto ires = m_countedNodes.emplace( &object );

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
				it->nodes.emplace( buffer, pass, object );
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

		auto empty()const noexcept
		{
			return m_pipelines.empty();
		}

	private:
		std::unordered_set< NodeT const * > m_countedNodes;
		std::map< uint32_t, PipelineNodes > m_pipelines;
	};
}

#endif
