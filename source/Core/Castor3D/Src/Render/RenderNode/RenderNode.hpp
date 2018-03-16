/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNode_H___
#define ___C3D_RenderNode_H___

#include "BillboardRenderNode.hpp"
#include "DistanceRenderNode.hpp"
#include "MorphingRenderNode.hpp"
#include "ObjectRenderNode.hpp"
#include "PassRenderNode.hpp"
#include "SceneRenderNode.hpp"
#include "SkinningRenderNode.hpp"
#include "StaticRenderNode.hpp"

namespace castor3d
{
	DECLARE_MULTIMAP( double, StaticRenderNode *, StaticRenderNodePtrByDistance );
	DECLARE_MULTIMAP( double, SkinningRenderNode *, SkinningRenderNodePtrByDistance );
	DECLARE_MULTIMAP( double, MorphingRenderNode *, MorphingRenderNodePtrByDistance );
	DECLARE_MULTIMAP( double, BillboardRenderNode *, BillboardRenderNodePtrByDistance );
	DECLARE_VECTOR( StaticRenderNode *, StaticRenderNodePtr );
	DECLARE_VECTOR( SkinningRenderNode *, SkinningRenderNodePtr );
	DECLARE_VECTOR( MorphingRenderNode *, MorphingRenderNodePtr );
	DECLARE_VECTOR( BillboardRenderNode *, BillboardRenderNodePtr );
	DECLARE_MAP( RenderPipelineRPtr, StaticRenderNodePtrArray, StaticRenderNodesPtrByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, SkinningRenderNodePtrArray, SkinningRenderNodesPtrByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, MorphingRenderNodePtrArray, MorphingRenderNodesPtrByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, BillboardRenderNodePtrArray, BillboardRenderNodesPtrByPipeline );
	DECLARE_MAP( SubmeshRPtr, StaticRenderNodePtrArray, SubmeshStaticRenderNodesPtr );
	DECLARE_MAP( SubmeshRPtr, SkinningRenderNodePtrArray, SubmeshSkinningRenderNodesPtr );
	DECLARE_MAP( PassRPtr, SubmeshStaticRenderNodesPtrMap, SubmeshStaticRenderNodesPtrByPass );
	DECLARE_MAP( RenderPipelineRPtr, SubmeshStaticRenderNodesPtrByPassMap, SubmeshStaticRenderNodesPtrByPipeline );
	DECLARE_MAP( PassRPtr, SubmeshSkinningRenderNodesPtrMap, SubmeshSkinninRenderNodesPtrByPass );
	DECLARE_MAP( RenderPipelineRPtr, SubmeshSkinninRenderNodesPtrByPassMap, SubmeshSkinningRenderNodesPtrByPipeline );

	DECLARE_MULTIMAP( double, StaticRenderNode, StaticRenderNodeByDistance );
	DECLARE_MULTIMAP( double, SkinningRenderNode, SkinningRenderNodeByDistance );
	DECLARE_MULTIMAP( double, MorphingRenderNode, MorphingRenderNodeByDistance );
	DECLARE_MULTIMAP( double, BillboardRenderNode, BillboardRenderNodeByDistance );
	DECLARE_VECTOR( StaticRenderNode, StaticRenderNode );
	DECLARE_VECTOR( SkinningRenderNode, SkinningRenderNode );
	DECLARE_VECTOR( MorphingRenderNode, MorphingRenderNode );
	DECLARE_VECTOR( BillboardRenderNode, BillboardRenderNode );
	DECLARE_MAP( RenderPipelineRPtr, StaticRenderNodeArray, StaticRenderNodesByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, SkinningRenderNodeArray, SkinningRenderNodesByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, MorphingRenderNodeArray, MorphingRenderNodesByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, BillboardRenderNodeArray, BillboardRenderNodesByPipeline );
	DECLARE_MAP( SubmeshRPtr, StaticRenderNodeArray, SubmeshStaticRenderNodes );
	DECLARE_MAP( SubmeshRPtr, SkinningRenderNodeArray, SubmeshSkinningRenderNodes );
	DECLARE_MAP( PassRPtr, SubmeshStaticRenderNodesMap, SubmeshStaticRenderNodesByPass );
	DECLARE_MAP( RenderPipelineRPtr, SubmeshStaticRenderNodesByPassMap, SubmeshStaticRenderNodesByPipeline );
	DECLARE_MAP( PassRPtr, SubmeshSkinningRenderNodesMap, SubmeshSkinninRenderNodesByPass );
	DECLARE_MAP( RenderPipelineRPtr, SubmeshSkinninRenderNodesByPassMap, SubmeshSkinningRenderNodesByPipeline );
}

#endif
