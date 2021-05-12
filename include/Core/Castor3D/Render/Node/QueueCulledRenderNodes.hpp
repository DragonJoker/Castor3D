/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCulledRenderNodes_H___
#define ___C3D_SceneCulledRenderNodes_H___

#include "Castor3D/Render/Node/QueueRenderNodes.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/Command/CommandBuffer.hpp>

namespace castor3d
{
	struct QueueCulledRenderNodes
		: public castor::OwnedBy< RenderQueue const >
	{
		using StaticNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodePtrByPipelineMap >;
		using SkinnedNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodePtrByPipelineMap >;
		using InstantiatedStaticNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodesPtrByPipelineMap >;
		using InstantiatedSkinnedNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodesPtrByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< SubmeshRenderNode, SubmeshRenderNodePtrByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodePtrByPipelineMap >;

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

		C3D_API explicit QueueCulledRenderNodes( RenderQueue const & queue );

		C3D_API void parse();
		C3D_API void prepareCommandBuffers( RenderQueue const & queue
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors );
		C3D_API bool hasNodes()const;
	};
}

#endif
