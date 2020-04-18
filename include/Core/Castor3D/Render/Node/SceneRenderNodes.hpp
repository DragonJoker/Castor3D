/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneRenderNodes_H___
#define ___C3D_SceneRenderNodes_H___

#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Node/RenderNode.hpp"

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

	struct SceneRenderNodes
	{
		struct AnimatedObjects
		{
			AnimatedMeshSPtr mesh;
			AnimatedSkeletonSPtr skeleton;
		};

		using StaticNodesMap = RenderNodesT< StaticRenderNode, StaticRenderNodesByPipelineMap >;
		using SkinnedNodesMap = RenderNodesT< SkinningRenderNode, SkinningRenderNodesByPipelineMap >;
		using InstantiatedStaticNodesMap = RenderNodesT< StaticRenderNode, SubmeshStaticRenderNodesByPipelineMap >;
		using InstantiatedSkinnedNodesMap = RenderNodesT< SkinningRenderNode, SubmeshSkinningRenderNodesByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< MorphingRenderNode, MorphingRenderNodesByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap >;

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

		inline explicit SceneRenderNodes( Scene const & scene )
			: scene{ scene }
		{
		}

		C3D_API void parse( RenderQueue const & queue
			, ShadowMapLightTypeArray & shadowMaps );

		C3D_API void addRenderNode( PipelineFlags const & flags
			, AnimatedObjects const & animated
			, CulledSubmesh const & culledNode
			, Geometry & instance
			, Pass & pass
			, Submesh & submesh
			, RenderPass & renderPass );
		C3D_API void addRenderNode( PipelineFlags const & flags
			, CulledBillboard const & culledNode
			, Pass & pass
			, BillboardBase & billboard
			, RenderPass & renderPass );

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
