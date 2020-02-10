/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNodeModule_H___
#define ___C3D_RenderNodeModule_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Node */
	//@{

	/**
	*\~english
	*\brief
	*	Helper structure used to render billboards.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des billboards.
	*/
	struct BillboardRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render distance sorted nodes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin de noeuds triés par distance.
	*/
	struct DistanceRenderNodeBase;
	/**
	*\~english
	*\brief
	*	Helper structure used to render distance sorted typed nodes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin de noeuds typés, triés par distance.
	*/
	template< typename NodeType >
	struct DistanceRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render animated submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages animés.
	*/
	struct MorphingRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render objects.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin d'objets.
	*/
	template< typename DataTypeT, typename InstanceTypeT >
	struct ObjectRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used link a pass and a shader program.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour lier une passe et un programme shader.
	*/
	struct PassRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages.
	*/
	struct SceneRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render skeleton animated submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages animés par squelette.
	*/
	struct SkinningRenderNode;
	/**
	*\~english
	*\brief
	*	Helper structure used to render static submeshes.
	*\~french
	*\brief
	*	Structure d'aide utilisée pour le dessin des sous-maillages non animés.
	*/
	struct StaticRenderNode;

	using SubmeshRenderNode = ObjectRenderNode< Submesh, Geometry >;
	using BillboardListRenderNode = ObjectRenderNode< BillboardBase, BillboardBase >;

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
	CU_DeclareMap( CulledSubmesh const *, StaticRenderNode, StaticRenderNode );
	CU_DeclareMap( CulledSubmesh const *, SkinningRenderNode, SkinningRenderNode );
	CU_DeclareMap( CulledSubmesh const *, MorphingRenderNode, MorphingRenderNode );
	CU_DeclareMap( CulledBillboard const *, BillboardRenderNode, BillboardRenderNode );
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

	//@}
	//@}
}

#endif
