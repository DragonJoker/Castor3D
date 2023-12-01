/*
See LICENSE file in root folder
*/
#ifndef ___C3D_QueueRenderNodes_H___
#define ___C3D_QueueRenderNodes_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Node/InstantiatedPipelinesNodes.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
#if VK_EXT_mesh_shader
	using DrawMeshTesksIndexedCommand = VkDrawMeshTasksIndirectCommandEXT;
#elif VK_NV_mesh_shader
	using DrawMeshTesksIndexedCommand = VkDrawMeshTasksIndirectCommandNV;
#endif
	struct QueueRenderNodes
		: public castor::OwnedBy< RenderQueue const >
	{
	public:
		using PipelineMap = std::unordered_map< size_t, PipelineAndID >;

	public:
		C3D_API explicit QueueRenderNodes( RenderQueue const & queue );

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		C3D_API void clear();
		C3D_API void checkEmpty();
		C3D_API void sortNodes( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer );
		C3D_API void updateNodes( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer );
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
		C3D_API void registerPipelineId( uint32_t pipelineId );
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
		PipelineAndID doGetPipeline( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, SubmeshRenderNode const & node
			, bool frontCulled );
		PipelineAndID doGetPipeline( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, BillboardRenderNode const & node );
		void doAddSubmesh( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CountedNodeT< SubmeshRenderNode > const & counted );
		void doAddBillboard( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CountedNodeT< BillboardRenderNode > const & counted );
		void doAddSingleSubmesh( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CountedNodeT< SubmeshRenderNode > const & node
			, bool frontCulled );
		void doAddInstancedSubmesh( ShadowMapLightTypeArray & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CountedNodeT< SubmeshRenderNode > const & node
			, bool frontCulled );
		void doAddSubmesh( CountedNodeT< SubmeshRenderNode > const & node );
		void doAddBillboard( CountedNodeT< BillboardRenderNode > const & node );
		void doRemoveSubmesh( CountedNodeT< SubmeshRenderNode > const & node );
		void doRemoveBillboard( CountedNodeT< BillboardRenderNode > const & node );
		uint32_t doPrepareMeshTraditionalCommandBuffers( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount );
		uint32_t doPrepareMeshModernCommandBuffers( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount );
		uint32_t doPrepareBillboardCommandBuffers( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount );

	private:
		PipelineBufferArray m_nodesIds;
		uint32_t m_maxPipelineId;
		bool m_hasNodes{};

#if VK_EXT_mesh_shader || VK_NV_mesh_shader
		using IndexedMeshDrawCommandsBuffer = ashes::BufferPtr< DrawMeshTesksIndexedCommand >;
		IndexedMeshDrawCommandsBuffer m_submeshMeshletIndirectCommands;
#endif
		using IndexedDrawCommandsBuffer = ashes::BufferPtr< VkDrawIndexedIndirectCommand >;
		IndexedDrawCommandsBuffer m_submeshIdxIndirectCommands;

		using DrawCommandsBuffer = ashes::BufferPtr< VkDrawIndirectCommand >;
		DrawCommandsBuffer m_submeshNIdxIndirectCommands;
		DrawCommandsBuffer m_billboardIndirectCommands;

		using PipelineNodesBuffer = ashes::BufferPtr< PipelineNodes >;
		PipelineNodesBuffer m_pipelinesNodes;

		SceneCullerSubmeshSignalConnection m_onSubmeshChanged;
		SceneCullerBillboardSignalConnection m_onBillboardChanged;

		std::unordered_set< CountedNodeT< SubmeshRenderNode > const * > m_pendingSubmeshes;
		std::unordered_set< CountedNodeT< BillboardRenderNode > const * > m_pendingBillboards;

		PipelineMap m_pipelines;
		RenderCounts m_visible{};
		PipelinesNodesT< SubmeshRenderNode > m_submeshNodes;
		InstantiatedPipelinesNodesT< SubmeshRenderNode > m_instancedSubmeshNodes;
		PipelinesNodesT< BillboardRenderNode > m_billboardNodes;
	};
}

#endif
