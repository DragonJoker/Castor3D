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

namespace crg::ru
{
	struct Config;
}

namespace castor3d
{
	struct QueueRenderNodes
		: public castor::OwnedBy< RenderQueue const >
	{
	public:
		using PipelineMap = castor::UnorderedMap< size_t, PipelineAndID >;

	public:
		C3D_API explicit QueueRenderNodes( RenderQueue const & queue
			, RenderDevice const & device
			, castor::String const & typeName
			, bool meshShading );
		C3D_API ~QueueRenderNodes()noexcept;
		C3D_API void fillConfig( crg::ru::Config & config )const;
		C3D_API void clear()noexcept;
		C3D_API void checkEmpty();
		C3D_API bool sortNodes( ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer );
		C3D_API bool updateNodes( ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer );
		C3D_API uint32_t prepareCommandBuffers( ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, ashes::CommandBuffer const & commandBuffer );

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
			, ashes::BufferBase const & posBuffer
			, ashes::BufferBase const * idxBuffer )const;
		C3D_API uint32_t getPipelineNodesIndex( Submesh const & submesh
			, Pass const & pass
			, ashes::BufferBase const & posBuffer
			, ashes::BufferBase const * idxBuffer
			, bool isFrontCulled )const;
		C3D_API uint32_t getPipelineNodesIndex( BillboardBase const & billboard
			, Pass const & pass
			, ashes::BufferBase const & posBuffer
			, ashes::BufferBase const * idxBuffer
			, bool isFrontCulled )const;

		auto & getNodesIds()const noexcept
		{
			return *m_pipelinesNodes;
		}

		auto & getSubmeshNodes()const noexcept
		{
			return m_submeshNodes;
		}

		auto & getInstancedSubmeshNodes()const noexcept
		{
			return m_instancedSubmeshNodes;
		}

		auto & getBillboardNodes()const noexcept
		{
			return m_billboardNodes;
		}

		bool hasCulledNodes()const noexcept
		{
			return m_hasNodes;
		}

		RenderCounts const & getVisibleCounts()const noexcept
		{
			return m_visible;
		}

	private:
		PipelineAndID doGetPipeline( ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
			, SubmeshRenderNode const & node
			, bool frontCulled );
		PipelineAndID doGetPipeline( ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, BillboardRenderNode const & node );
		void doAddSubmesh( ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CulledNodeT< SubmeshRenderNode > const & counted );
		void doAddBillboard( ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CulledNodeT< BillboardRenderNode > const & counted );
		void doAddSingleSubmesh( ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CulledNodeT< SubmeshRenderNode > const & node
			, bool frontCulled );
		void doAddInstancedSubmesh( ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer
			, CulledNodeT< SubmeshRenderNode > const & node
			, bool frontCulled );
		void doAddSubmesh( CulledNodeT< SubmeshRenderNode > const & node );
		void doAddBillboard( CulledNodeT< BillboardRenderNode > const & node );
		void doRemoveSubmesh( CulledNodeT< SubmeshRenderNode > const & node );
		void doRemoveBillboard( CulledNodeT< BillboardRenderNode > const & node );
		uint32_t doParseSimpleSubmeshesDirect( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount
			, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & idxIndex
			, uint32_t & nidxIndex );
		uint32_t doParseInstantiatedSubmeshesDirect( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & idxIndex
			, uint32_t & nidxIndex );
		uint32_t doParseSimpleSubmeshesIndirect( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
			, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, ashes::Buffer< VkDrawIndirectCommand > const & submeshNIdxCommands
			, VkDrawIndirectCommand * origIndirectNIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & idxIndex
			, uint32_t & nidxIndex );
		uint32_t doParseInstantiatedSubmeshesIndirect( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
			, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, ashes::Buffer< VkDrawIndirectCommand > const & submeshNIdxCommands
			, VkDrawIndirectCommand * origIndirectNIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & idxIndex
			, uint32_t & nidxIndex );
#if VK_EXT_mesh_shader
		uint32_t doParseSimpleSubmeshesMeshletsEXT( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
			, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, ashes::Buffer< VkDrawIndirectCommand > const & submeshNIdxCommands
			, VkDrawIndirectCommand * origIndirectNIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandEXT > const & submeshMshCommands
			, VkDrawMeshTasksIndirectCommandEXT * origIndirectMshBuffer
			, VkDrawMeshTasksIndirectCommandEXT *& indirectMshBuffer
			, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & mshIndex );
		uint32_t doParseInstantiatedSubmeshesMeshletsEXT( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
			, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, ashes::Buffer< VkDrawIndirectCommand > const & submeshNIdxCommands
			, VkDrawIndirectCommand * origIndirectNIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandEXT > const & submeshMshCommands
			, VkDrawMeshTasksIndirectCommandEXT * origIndirectMshBuffer
			, VkDrawMeshTasksIndirectCommandEXT *& indirectMshBuffer
			, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & mshIndex );
#endif
#if VK_NV_mesh_shader
		uint32_t doParseSimpleSubmeshesMeshletsNV( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
			, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, ashes::Buffer< VkDrawIndirectCommand > const & submeshNIdxCommands
			, VkDrawIndirectCommand * origIndirectNIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & submeshMshCommands
			, VkDrawMeshTasksIndirectCommandNV * origIndirectMshBuffer
			, VkDrawMeshTasksIndirectCommandNV *& indirectMshBuffer
			, BuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & mshIndex );
		uint32_t doParseInstantiatedSubmeshesMeshletsNV( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, ashes::Buffer< VkDrawIndexedIndirectCommand > const & submeshIdxCommands
			, VkDrawIndexedIndirectCommand * origIndirectIdxBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, ashes::Buffer< VkDrawIndirectCommand > const & submeshNIdxCommands
			, VkDrawIndirectCommand * origIndirectNIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, ashes::Buffer< VkDrawMeshTasksIndirectCommandNV > const & submeshMshCommands
			, VkDrawMeshTasksIndirectCommandNV * origIndirectMshBuffer
			, VkDrawMeshTasksIndirectCommandNV *& indirectMshBuffer
			, InstantiatedBuffersNodesViewT< SubmeshRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & mshIndex );
#endif
		uint32_t doParseBillboardsDirect( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount
			, BuffersNodesViewT< BillboardRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & nidxIndex );
		uint32_t doParseBillboardsIndirect( ashes::CommandBuffer const & commandBuffer
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors
			, PipelineNodes * nodesIdsBuffer
			, VkDeviceSize maxNodesCount
			, ashes::Buffer< VkDrawIndirectCommand > const & billboardCommands
			, VkDrawIndirectCommand * origIndirectBuffer
			, VkDrawIndirectCommand *& indirectBuffer
			, BuffersNodesViewT< BillboardRenderNode > const & buffersNodes
			, RenderPipeline const & pipeline
			, uint32_t & idxIndex
			, uint32_t & nidxIndex );

	private:
		PipelineBufferArray m_nodesIds;
		uint32_t m_maxPipelineId{};
		bool m_hasNodes{};

#if VK_NV_mesh_shader
		using IndexedMeshDrawCommandsBufferNV = ashes::BufferPtr< VkDrawMeshTasksIndirectCommandNV >;
		IndexedMeshDrawCommandsBufferNV m_submeshMeshletIndirectCommandsNV;
#endif
#if VK_EXT_mesh_shader
		using IndexedMeshDrawCommandsBufferEXT = ashes::BufferPtr< VkDrawMeshTasksIndirectCommandEXT >;
		IndexedMeshDrawCommandsBufferEXT m_submeshMeshletIndirectCommandsEXT;
#endif
		using IndexedDrawCommandsBuffer = ashes::BufferPtr< VkDrawIndexedIndirectCommand >;
		IndexedDrawCommandsBuffer m_submeshIdxIndirectCommands;

		using DrawCommandsBuffer = ashes::BufferPtr< VkDrawIndirectCommand >;
		DrawCommandsBuffer m_submeshNIdxIndirectCommands;
		DrawCommandsBuffer m_billboardIndirectCommands;

		using PipelineNodesBuffer = ashes::BufferPtr< PipelineNodes >;
		PipelineNodesBuffer m_pipelinesNodes;

		SceneCullerSubmeshSignalConnection m_onSubmeshChanged;
		SceneCullerSubmeshSignalConnection m_onSubmeshRemoved;
		SceneCullerBillboardSignalConnection m_onBillboardChanged;
		SceneCullerBillboardSignalConnection m_onBillboardRemoved;

		castor::UnorderedSet< CulledNodeT< SubmeshRenderNode > const * > m_pendingSubmeshes;
		castor::UnorderedSet< CulledNodeT< BillboardRenderNode > const * > m_pendingBillboards;

		PipelineMap m_pipelines;
		RenderCounts m_visible{};
		PipelinesNodesT< SubmeshRenderNode > m_submeshNodes;
		InstantiatedPipelinesNodesT< SubmeshRenderNode > m_instancedSubmeshNodes;
		PipelinesNodesT< BillboardRenderNode > m_billboardNodes;
	};
}

#endif
