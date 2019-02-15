/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNode_H___
#define ___C3D_RenderNode_H___

#include "Castor3D/Render/RenderNode/BillboardRenderNode.hpp"
#include "Castor3D/Render/RenderNode/DistanceRenderNode.hpp"
#include "Castor3D/Render/RenderNode/MorphingRenderNode.hpp"
#include "Castor3D/Render/RenderNode/ObjectRenderNode.hpp"
#include "Castor3D/Render/RenderNode/PassRenderNode.hpp"
#include "Castor3D/Render/RenderNode/SceneRenderNode.hpp"
#include "Castor3D/Render/RenderNode/SkinningRenderNode.hpp"
#include "Castor3D/Render/RenderNode/StaticRenderNode.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"

namespace castor3d
{
	CU_DeclareMultimap( double, StaticRenderNode *, StaticRenderNodePtrByDistance );
	CU_DeclareMultimap( double, SkinningRenderNode *, SkinningRenderNodePtrByDistance );
	CU_DeclareMultimap( double, MorphingRenderNode *, MorphingRenderNodePtrByDistance );
	CU_DeclareMultimap( double, BillboardRenderNode *, BillboardRenderNodePtrByDistance );
	CU_DeclareVector( StaticRenderNode *, StaticRenderNodePtr );
	CU_DeclareVector( SkinningRenderNode *, SkinningRenderNodePtr );
	CU_DeclareVector( MorphingRenderNode *, MorphingRenderNodePtr );
	CU_DeclareVector( BillboardRenderNode *, BillboardRenderNodePtr );
	CU_DeclareMap( RenderPipelineRPtr, StaticRenderNodePtrArray, StaticRenderNodesPtrByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, SkinningRenderNodePtrArray, SkinningRenderNodesPtrByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, MorphingRenderNodePtrArray, MorphingRenderNodesPtrByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, BillboardRenderNodePtrArray, BillboardRenderNodesPtrByPipeline );
	CU_DeclareMap( SubmeshRPtr, StaticRenderNodePtrArray, SubmeshStaticRenderNodesPtr );
	CU_DeclareMap( SubmeshRPtr, SkinningRenderNodePtrArray, SubmeshSkinningRenderNodesPtr );
	CU_DeclareMap( PassRPtr, SubmeshStaticRenderNodesPtrMap, SubmeshStaticRenderNodesPtrByPass );
	CU_DeclareMap( PassRPtr, SubmeshSkinningRenderNodesPtrMap, SubmeshSkinninRenderNodesPtrByPass );
	CU_DeclareMap( RenderPipelineRPtr, SubmeshStaticRenderNodesPtrByPassMap, SubmeshStaticRenderNodesPtrByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, SubmeshSkinninRenderNodesPtrByPassMap, SubmeshSkinningRenderNodesPtrByPipeline );

	CU_DeclareMultimap( double, StaticRenderNode, StaticRenderNodeByDistance );
	CU_DeclareMultimap( double, SkinningRenderNode, SkinningRenderNodeByDistance );
	CU_DeclareMultimap( double, MorphingRenderNode, MorphingRenderNodeByDistance );
	CU_DeclareMultimap( double, BillboardRenderNode, BillboardRenderNodeByDistance );
	CU_DeclareMap( SceneCuller::CulledSubmesh const *, StaticRenderNode, StaticRenderNode );
	CU_DeclareMap( SceneCuller::CulledSubmesh const *, SkinningRenderNode, SkinningRenderNode );
	CU_DeclareMap( SceneCuller::CulledSubmesh const *, MorphingRenderNode, MorphingRenderNode );
	CU_DeclareMap( SceneCuller::CulledBillboard const *, BillboardRenderNode, BillboardRenderNode );
	CU_DeclareMap( RenderPipelineRPtr, StaticRenderNodeMap, StaticRenderNodesByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, SkinningRenderNodeMap, SkinningRenderNodesByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, MorphingRenderNodeMap, MorphingRenderNodesByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, BillboardRenderNodeMap, BillboardRenderNodesByPipeline );
	CU_DeclareMap( SubmeshRPtr, StaticRenderNodeMap, SubmeshStaticRenderNodes );
	CU_DeclareMap( SubmeshRPtr, SkinningRenderNodeMap, SubmeshSkinningRenderNodes );
	CU_DeclareMap( PassRPtr, SubmeshStaticRenderNodesMap, SubmeshStaticRenderNodesByPass );
	CU_DeclareMap( PassRPtr, SubmeshSkinningRenderNodesMap, SubmeshSkinninRenderNodesByPass );
	CU_DeclareMap( RenderPipelineRPtr, SubmeshStaticRenderNodesByPassMap, SubmeshStaticRenderNodesByPipeline );
	CU_DeclareMap( RenderPipelineRPtr, SubmeshSkinninRenderNodesByPassMap, SubmeshSkinningRenderNodesByPipeline );
}

#endif
