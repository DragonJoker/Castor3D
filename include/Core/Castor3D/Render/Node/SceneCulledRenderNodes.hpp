/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneCulledRenderNodes_H___
#define ___C3D_SceneCulledRenderNodes_H___

#include "Castor3D/Render/Node/SceneRenderNodes.hpp"

#include <ashespp/Command/CommandBuffer.hpp>

namespace castor3d
{
	struct SceneCulledRenderNodes
	{
		using StaticNodesMap = RenderNodesT< StaticRenderNode, StaticRenderNodePtrByPipelineMap >;
		using SkinnedNodesMap = RenderNodesT< SkinningRenderNode, SkinningRenderNodePtrByPipelineMap >;
		using InstantiatedStaticNodesMap = RenderNodesT< StaticRenderNode, SubmeshStaticRenderNodesPtrByPipelineMap >;
		using InstantiatedSkinnedNodesMap = RenderNodesT< SkinningRenderNode, SubmeshSkinningRenderNodesPtrByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< MorphingRenderNode, MorphingRenderNodePtrByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodePtrByPipelineMap >;

		//!\~english	The scene.
		//!\~french		La scène.
		Scene const & scene;
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

		inline explicit SceneCulledRenderNodes( Scene const & scene )
			: scene{ scene }
		{
		}

		C3D_API void parse( RenderQueue const & queue );
		C3D_API void prepareCommandBuffers( RenderQueue const & queue
			, ashes::Optional< VkViewport > const & viewport
			, ashes::Optional< VkRect2D > const & scissors );

		inline bool hasNodes()const
		{
			return !staticNodes.backCulled.empty()
				|| !staticNodes.frontCulled.empty()
				|| !skinnedNodes.backCulled.empty()
				|| !skinnedNodes.frontCulled.empty()
				|| !instancedStaticNodes.backCulled.empty()
				|| !instancedStaticNodes.frontCulled.empty()
				|| !instancedSkinnedNodes.backCulled.empty()
				|| !instancedSkinnedNodes.frontCulled.empty()
				|| !morphingNodes.backCulled.empty()
				|| !morphingNodes.frontCulled.empty()
				|| !billboardNodes.backCulled.empty()
				|| !billboardNodes.frontCulled.empty();
		}
	};
}

#endif
