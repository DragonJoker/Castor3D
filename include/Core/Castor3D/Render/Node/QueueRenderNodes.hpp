/*
See LICENSE file in root folder
*/
#ifndef ___C3D_QueueRenderNodes_H___
#define ___C3D_QueueRenderNodes_H___

#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	template< typename NodeType, typename MapType >
	struct RenderNodesT
	{
		//!\~english	The geometries, sorted by shader program.
		//!\~french		Les géométries, triées par programme shader.
		MapType frontCulled;
		//!\~english	The geometries, sorted by shader program.
		//!\~french		Les géométries, triées par programme shader.
		MapType backCulled;
	};

	struct QueueRenderNodes
		: public castor::OwnedBy< RenderQueue const >
	{
		struct AnimatedObjects
		{
			AnimatedMeshSPtr mesh;
			AnimatedSkeletonSPtr skeleton;
		};

		using StaticNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodeByPipelineMap >;
		using SkinnedNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodeByPipelineMap >;
		using InstantiatedStaticNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodesByPipelineMap >;
		using InstantiatedSkinnedNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodesByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodeByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodeByPipelineMap >;

		//!\~english	The static render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu statiques, triés par programme shader.
		StaticNodesMap staticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		SkinnedNodesMap skinnedNodes;
		//!\~english	The instanced render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu instanciés, triés par programme shader.
		InstantiatedStaticNodesMap instancedStaticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		InstantiatedSkinnedNodesMap instancedSkinnedNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		MorphingNodesMap morphingNodes;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		BillboardNodesMap billboardNodes;

		C3D_API QueueRenderNodes( RenderQueue const & queue );

		C3D_API void parse( ShadowMapLightTypeArray & shadowMaps );
		C3D_API void addRenderNode( PipelineFlags const & flags
			, AnimatedObjects const & animated
			, CulledSubmesh const & culledNode
			, Geometry & instance
			, Pass & pass
			, Submesh & submesh
			, SceneRenderPass & renderPass );
		C3D_API void addRenderNode( PipelineFlags const & flags
			, CulledBillboard const & culledNode
			, Pass & pass
			, BillboardBase & billboard
			, SceneRenderPass & renderPass );
		C3D_API bool hasNodes()const;

		C3D_API SubmeshRenderNode & createNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, Submesh & data
			, Geometry & instance
			, AnimatedMesh * mesh
			, AnimatedSkeleton * skeleton );
		C3D_API BillboardRenderNode & createNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, BillboardBase & instance
			, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer );
		C3D_API ashes::DescriptorSetLayoutCRefArray getDescriptorSetLayouts( Pass const & pass
			, BillboardBase const & billboard );
		C3D_API ashes::DescriptorSetLayoutCRefArray getDescriptorSetLayouts( Pass const & pass
			, Submesh const & submesh
			, AnimatedMesh const * mesh
			, AnimatedSkeleton const * skeleton );

	private:
		SceneRenderNodes m_allNodes;
	};
}

#endif
