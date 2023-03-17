/*
See LICENSE file in root folder
*/
#ifndef ___C3D_QueueRenderNodes_H___
#define ___C3D_QueueRenderNodes_H___

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
	struct QueueRenderNodes
		: public castor::OwnedBy< RenderQueue const >
	{
	public:
		C3D_API explicit QueueRenderNodes( RenderQueue const & queue );

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		C3D_API void checkEmpty();
		C3D_API void sortNodes( ShadowMapLightTypeArray & shadowMaps );
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

		C3D_API std::pair< uint32_t, uint32_t > fillPipelinesIds( castor::ArrayView< uint32_t > nodesPipelinesIds )const;
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

	private:
		RenderPipeline & doGetPipeline( SubmeshRenderNode const & node
			, bool frontCulled );
		RenderPipeline & doGetPipeline( BillboardRenderNode const & node );
		void doAddSubmesh( ShadowMapLightTypeArray & shadowMaps
			, SubmeshRenderNode const & node
			, bool frontCulled );
		void doAddInstancedSubmesh( ShadowMapLightTypeArray & shadowMaps
			, SubmeshRenderNode const & node
			, bool frontCulled );
		void doAddBillboard( ShadowMapLightTypeArray & shadowMaps
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

		std::unordered_map< size_t, RenderPipeline * > m_pipelines;

		//!\~english	The submesh render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de submesh, triés par programme shader.
		NodePtrByPipelineMapT< SubmeshRenderNode > m_submeshNodes;
		//!\~english	The submesh render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de submesh, triés par programme shader.
		ObjectNodesPtrByPipelineMapT< SubmeshRenderNode > m_instancedSubmeshNodes;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		NodePtrByPipelineMapT< BillboardRenderNode > m_billboardNodes;
	};
}

#endif
