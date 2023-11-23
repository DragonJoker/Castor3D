/*
See LICENSE file in root folder
*/
#ifndef ___C3D_QueueRenderNodes_H___
#define ___C3D_QueueRenderNodes_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	C3D_API VkDeviceSize compareOffsets( SubmeshRenderNode const & lhs
		, SubmeshRenderNode const & rhs );
	C3D_API VkDeviceSize compareOffsets( BillboardRenderNode const & lhs
		, BillboardRenderNode const & rhs );

	template< typename NodeT >
	class NodePtrByPipelineMapT
	{
	public:
		static uint64_t constexpr maxNodes = 1'024ull;
		static uint64_t constexpr maxBuffers = 128ull;
		static uint64_t constexpr maxPipelines = 128ull;

		using CountedNode = CountedNodeT< NodeT >;
		using NodeArray = NodeArrayT< NodeT >;

		struct FilledNodes
		{
			explicit FilledNodes( CountedNode * data )
				: m_nodes{ data }
				, m_count{}
			{
			}

			FilledNodes()
				: FilledNodes{ nullptr }
			{
			}

			auto emplace( NodeT const & node
				, uint32_t drawCount
				, bool isFrontCulled )
			{
				//auto it = std::find_if( begin()
				//	, end()
				//	, [&node]( CountedNode const & lookup )
				//	{
				//		return compareOffsets( *lookup.node, node );
				//	} );
				//auto offset = size_t( std::distance( begin(), it ) );

				//while ( offset < m_count )
				//{
				//	m_nodes[offset + 1u] = m_nodes[offset];
				//	++offset;
				//}

				auto it = end();
				*it = CountedNode{ &node, drawCount, isFrontCulled };
				++m_count;
			}

			void clear()noexcept
			{
				m_count = 0u;
			}

			auto begin()noexcept
			{
				return m_nodes;
			}

			auto begin()const noexcept
			{
				return m_nodes;
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
			CountedNode * m_nodes;
			size_t m_count;
		};

		struct FilledBuffers
		{
			struct BufferNodes
			{
				ashes::BufferBase const * buffer{};
				FilledNodes nodes;
			};

			explicit FilledBuffers( CountedNode * data )
				: m_count{}
			{
				if ( data )
				{
					for ( auto & buffer : m_buffers )
					{
						buffer.nodes = FilledNodes{ data };
						data += maxNodes;
					}
				}
			}

			FilledBuffers()
				: FilledBuffers{ nullptr }
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
					it->buffer = &buffer;
					++m_count;
				}

				return it;
			}

			void emplace( ashes::BufferBase const & buffer
				, NodeT const & node
				, uint32_t drawCount
				, bool isFrontCulled )
			{
				auto it = emplace( buffer );
				it->nodes.emplace( node, drawCount, isFrontCulled );
				CU_Require( !it->nodes.empty() );
			}

			void clear()noexcept
			{
				for ( auto & [buffer, nodes] : m_buffers )
				{
					buffer = nullptr;
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
			std::array< BufferNodes, maxBuffers > m_buffers;
			uint32_t m_count;
		};

		struct PipelineNodes
		{
			RenderPipeline const * pipeline{};
			FilledBuffers buffers;
		};

		NodePtrByPipelineMapT()
			: m_nodes{ maxNodes * maxBuffers * maxPipelines }
			, m_count{}
		{
			auto data = m_nodes.data();

			for ( auto & pipeline : m_pipelines )
			{
				pipeline.buffers = FilledBuffers{ data };
				data += maxNodes * maxBuffers;
			}
		}

		auto emplace( RenderPipeline & pipeline )
		{
			auto it = std::find_if( begin()
				, end()
				, [&pipeline]( PipelineNodes const & lookup )
				{
					return lookup.pipeline == &pipeline;
				} );

			if ( it == end() )
			{
				it->pipeline = &pipeline;
				++m_count;
			}

			return it;
		}

		void emplace( RenderPipeline & pipeline
			, ashes::BufferBase const & buffer
			, NodeT const & node
			, uint32_t drawCount
			, bool isFrontCulled )
		{
			auto it = emplace( pipeline );
			it->buffers.emplace( buffer, node, drawCount, isFrontCulled );
		}

		void clear()noexcept
		{
			for ( auto & [pipeline, buffers] : m_pipelines )
			{
				pipeline = nullptr;
				buffers.clear();
			}

			m_count = 0u;
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
		std::array< PipelineNodes, maxPipelines > m_pipelines;
		uint32_t m_count;
	};

	struct QueueRenderNodes
		: public castor::OwnedBy< RenderQueue const >
	{
	public:
		using PipelineMap = std::unordered_map< size_t, RenderPipeline * >;

	public:
		C3D_API explicit QueueRenderNodes( RenderQueue const & queue );

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		C3D_API void clear();
		C3D_API void checkEmpty();
		C3D_API void sortNodes( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer );
		C3D_API void fillIndirectBuffers();
		C3D_API uint32_t prepareCommandBuffers( ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors );

		C3D_API SubmeshRenderNode & createNode( Pass & pass
			, Submesh & data
			, Geometry & instance
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		C3D_API BillboardRenderNode & createNode( Pass & pass
			, BillboardBase & instance );

		C3D_API uint32_t getMaxPipelineId()const;
		C3D_API void registerNodePipeline( uint32_t nodeId
			, uint32_t pipelineId );
		C3D_API PipelineBufferArray const & getPassPipelineNodes()const;
		C3D_API uint32_t getPipelineNodesIndex( PipelineBaseHash const & hash
			, ashes::BufferBase const & buffer )const;
		C3D_API uint32_t getPipelineNodesIndex( Submesh const & submesh
			, Pass const & pass
			, ashes::BufferBase const & buffer
			, bool isFrontCulled )const;
		C3D_API uint32_t getPipelineNodesIndex( BillboardBase const & billboard
			, Pass const & pass
			, ashes::BufferBase const & buffer
			, bool isFrontCulled )const;

		auto & getNodesIds()const
		{
			return *m_pipelinesNodes;
		}

		auto & getSubmeshNodes()const
		{
			return m_submeshNodes;
		}

		auto & getInstancedSubmeshNodes()const
		{
			return m_instancedSubmeshNodes;
		}

		auto & getBillboardNodes()const
		{
			return m_billboardNodes;
		}

		bool hasCulledNodes()const
		{
			return m_hasNodes;
		}

		RenderCounts const & getVisibleCounts()const noexcept
		{
			return m_visible;
		}

	private:
		RenderPipeline & doGetPipeline( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, SubmeshRenderNode const & node
			, bool frontCulled );
		RenderPipeline & doGetPipeline( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, BillboardRenderNode const & node );
		void doAddSubmesh( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, SubmeshRenderNode const & node
			, bool frontCulled );
		void doAddInstancedSubmesh( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, SubmeshRenderNode const & node
			, bool frontCulled );
		void doAddBillboard( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, BillboardRenderNode const & node );

	private:
		PipelineBufferArray m_nodesIds;
		std::map< uint32_t, uint32_t > m_nodesPipelinesIds;
		bool m_hasNodes{};

#if VK_NV_mesh_shader
		using IndexedMeshDrawCommandsBuffer = ashes::BufferPtr< VkDrawMeshTasksIndirectCommandNV >;
		IndexedMeshDrawCommandsBuffer m_submeshMeshletIndirectCommands;
#endif
		using IndexedDrawCommandsBuffer = ashes::BufferPtr< VkDrawIndexedIndirectCommand >;
		IndexedDrawCommandsBuffer m_submeshIdxIndirectCommands;

		using DrawCommandsBuffer = ashes::BufferPtr< VkDrawIndirectCommand >;
		DrawCommandsBuffer m_submeshNIdxIndirectCommands;
		DrawCommandsBuffer m_billboardIndirectCommands;

		using PipelineNodesBuffer = ashes::BufferPtr< PipelineNodes >;
		PipelineNodesBuffer m_pipelinesNodes;

		PipelineMap m_pipelines;
		RenderCounts m_visible{};
		NodePtrByPipelineMapT< SubmeshRenderNode > m_submeshNodes;
		ObjectNodesPtrByPipelineMapT< SubmeshRenderNode > m_instancedSubmeshNodes;
		NodePtrByPipelineMapT< BillboardRenderNode > m_billboardNodes;
	};
}

#endif
